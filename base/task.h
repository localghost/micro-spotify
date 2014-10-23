#ifndef BASE_TASK_H
#define BASE_TASK_H

#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <exception>
#include <type_traits>
#include <utility>
#include <boost/scope_exit.hpp>
#include <base/assert.h>
#include <base/exception.h>
#include <base/compatibility.h>
#include <base/callable.h>
#include <base/thread.h>

namespace base {
template<typename> class task;

enum struct task_error_code
{
  no_state,
  cancelled,
  not_run,
  already_called,
  handle_already_acquired
};
typedef ::boost::error_info<struct task_error_code_tag, task_error_code> task_error_info;
EXCEPTION_TYPE(task_error);

// TODO Do I need specialization for T&?
// TODO Do I need specialization for void?
// TODO In fact it would be better to implement Small Shared State Optimization
//      instead of just differenting between integral and not integral types.
template<typename T, bool = std::is_integral<T>::value>
struct task_shared_state_traits;

template<typename T>
struct task_shared_state_traits<T, false>
{
  typedef std::unique_ptr<T> storage_type;
  typedef const T& lvalue_value_type;
  typedef T&& rvalue_value_type;

  static void set_value(storage_type& storage, lvalue_value_type value)
  {
    storage.reset(new T{value});
  }

  static void set_value(storage_type& storage, rvalue_value_type value)
  {
    storage.reset(new T{std::move(value)});
  }

  static T get_value(storage_type& storage)
  {
    BOOST_ASSERT(storage);
    return *storage.release();
  }
};

template<typename T>
struct task_shared_state_traits<T, true>
{
  typedef T storage_type;
  typedef const T& lvalue_value_type;
  typedef T&& rvalue_value_type;

  // Use this for rvalue as well (for now?)
  static void set_value(storage_type& storage, lvalue_value_type value)
  {
    storage = value;
  }

  static T get_value(storage_type& storage)
  {
    return std::move(storage);
  }
};

enum struct task_state { initialized, running, cancelled, finished };

class task_shared_state_base
{
public:
  task_shared_state_base() : state_{task_state::initialized} {}

  bool is_ready() const noexcept
  {
    return (task_state::cancelled == state_ || task_state::finished == state_);
  }

  void make_ready() noexcept
  {
    state_ = task_state::finished;
    wait_cv_.notify_all();
  }

  bool mark_running() noexcept
  {
    task_state expected = task_state::initialized;
    return state_.compare_exchange_strong(expected, task_state::running);
  }

  void wait() const
  {
    if (is_ready()) // premature optimization?
      return;

    std::unique_lock<std::mutex> lock(wait_mutex_);
    wait_cv_.wait(lock, std::bind(&task_shared_state_base::is_ready, this));
  }

  bool cancel() noexcept
  {
    task_state expected = task_state::initialized;
    return state_.compare_exchange_strong(expected, task_state::cancelled);
  }

  bool is_cancelled() const noexcept
  {
    return task_state::cancelled == state_;
  }

  void set_exception(std::exception_ptr exception) noexcept
  {
    exception_ = exception;
  }

  std::exception_ptr get_exception() const noexcept
  {
    return exception_;
  }

private:
  std::atomic<task_state> state_;
  mutable std::mutex wait_mutex_;
  mutable std::condition_variable wait_cv_;
  std::exception_ptr exception_;
};

template<typename R>
class task_shared_state : public task_shared_state_base
{
public:
  void set_result(typename task_shared_state_traits<R>::lvalue_value_type result)
  {
    task_shared_state_traits<R>::set_value(storage, result);
  }

  void set_result(typename task_shared_state_traits<R>::rvalue_value_type result)
  {
    task_shared_state_traits<R>::set_value(storage, std::move(result));
  }

  R get_result()
  {
    return task_shared_state_traits<R>::get_value(storage);
  }

private:
  typename task_shared_state_traits<R>::storage_type storage;
};

template<>
class task_shared_state<void> : public task_shared_state_base
{
public:
  void get_result() { }
}; 

template<typename R>
class task_handle FINAL
{
public:
  typedef R result_type;

  task_handle() = default;
  task_handle(const task_handle&) = delete;
  task_handle(task_handle&&) = default;

  void operator=(const task_handle&) = delete;
  task_handle& operator=(task_handle&&) = default;

  // TODO Is there a way to mitigate deadlock that occrs when a task is
  //      enqueued on current thread and get() on its handle is called?
  result_type get()
  {
    wait();

    BOOST_SCOPE_EXIT_ALL(this) { state_.reset(); };

    if (state_->is_cancelled())
      THROW(task_error{} << task_error_info{task_error_code::cancelled});

    std::exception_ptr exception = state_->get_exception();
    if (nullptr != exception)
      std::rethrow_exception(exception);

    return state_->get_result();
  }

  void wait() const
  {
    if (!state_)
      THROW(task_error{} << task_error_info{task_error_code::no_state});

    state_->wait();
  }

  /// Cancels the task
  /// @return Whether cancellation succeeded or not
  /// @throw base::task_error base::task_error_code::no_state
  bool cancel()
  {
    if (!state_)
      THROW(task_error{} << task_error_info{task_error_code::no_state});

    return state_->cancel();
  }

  /// Checks whether task has associated shared state with it
  /// @return True if task has associated shared state, false otherwise
  bool is_valid() const
  {
    return bool(state_);
  }

private:
  template<typename> friend class task;

  explicit task_handle(const std::shared_ptr<task_shared_state<result_type>>& state) noexcept
    : state_(state) { }

  std::shared_ptr<task_shared_state<result_type>> state_;
};

class continuation
{
private:
  struct base
  {
    virtual ~base() {}
    virtual callable move_out() = 0;
  };

  template<typename T>
  struct model : base
  {
    explicit model(T t) : callable_{std::move(t)} {}

    callable move_out()
    {
      return std::move(callable_);
    }

    T callable_;
  };

public:
  template<typename T>
  T& set_task(T task)
  {
    model<T>* m = new model<T>{std::move(task)};
    ptr_.reset(m);
    return m->callable_;
  }

  callable move_out()
  {
    return ptr_->move_out();
  }

  bool is_set() const
  {
    return ptr_.get() != nullptr;
  }

private:
  std::unique_ptr<base> ptr_;
};

template<typename R>
class task FINAL
{
public:
  typedef R result_type;

  task() = default;

  // This looks like a premature optimization, is it? Isn't std::bind() optimized already for callables
  // that don't take any arguments?
  template<typename F, typename std::enable_if<!std::is_same<task, F>::value>::type* = nullptr>
  explicit task(F&& callable)
    : callable_(std::forward<F>(callable)),
      state_(new task_shared_state<result_type>)
  { }

  template<typename F, typename ...Args>
  explicit task(F&& callable, Args&&... args)
    : callable_(std::bind(std::forward<F>(callable), std::forward<Args>(args)...)),
      state_(new task_shared_state<result_type>)
  { }

  task(const task&) = delete;
  task(task&& other)
  {
    continuation_ = std::move(other.continuation_);
    callable_ = std::move(other.callable_);
    state_ = std::move(other.state_);
    handle_acquired_.store(handle_acquired_.load(std::memory_order_relaxed), std::memory_order_relaxed); 
  }

  ~task()
  {
    if (state_)
    {
      if (handle_acquired_.load(std::memory_order_relaxed) && !state_->is_ready())
      {
        state_->set_exception(std::make_exception_ptr(
              task_error{} << task_error_info{task_error_code::not_run} << EXCEPTION_LOCATION));
        state_->make_ready();
      }
    }
  }

  task& operator=(const task&) = delete;
  task& operator=(task&& other)
  {
    using std::swap;
    task tmp = std::move(other);
    swap(tmp, *this);
    return *this;
  }

  task_handle<result_type> get_handle() const
  {
    if (!state_)
      THROW(task_error{} << task_error_info{task_error_code::no_state});

    bool expected = false;
    if (!handle_acquired_.compare_exchange_strong(expected, true))
      THROW(task_error{} << task_error_info{task_error_code::handle_already_acquired});

    return task_handle<result_type>{state_};
  }

  void operator()()
  {
    if (!state_)
      THROW(task_error{} << task_error_info{task_error_code::no_state});

    if (callable_)
    {
      if (!state_->mark_running())
      {
        // This should work as user can cancel only if task is in task_state::initialized state
        if (state_->is_cancelled()) return;
        THROW(task_error{} << task_error_info{task_error_code::already_called});
      }

      try
      {
        state_->set_result(callable_());
        post_continuation();
      }
      catch (...)
      {
          state_->set_exception(std::current_exception());
      }

      state_->make_ready();
    }
  }

  template<typename F>
  task<typename std::result_of<F()>::type>& then(F&& action)
  {
    task<typename std::result_of<F()>::type> t = std::forward<F>(action);
    return continuation_.set_task(std::move(t));
  }

private:
  void post_continuation()
  {
    if (continuation_.is_set())
      thread::current()->post_task(continuation_.move_out());
  }

  continuation continuation_;
  std::function<result_type()> callable_;
  std::shared_ptr<task_shared_state<result_type>> state_;
  mutable std::atomic<bool> handle_acquired_{false};
};

template<>
class task<void> FINAL
{
public:
  typedef void result_type;

  task() = default;

  template<typename F, typename std::enable_if<!std::is_same<task, F>::value>::type* = nullptr>
  explicit task(F&& callable)
    : callable_(std::forward<F>(callable)),
      state_(new task_shared_state<result_type>)
  { }

  template<typename F, typename ...Args>
  explicit task(F&& callable, Args&&... args) 
    : callable_(std::bind(std::forward<F>(callable), std::forward<Args>(args)...)),
      state_(new task_shared_state<result_type>)
  { }

  task(const task&) = delete;
  task(task&& other)
  {
    continuation_ = std::move(other.continuation_);
    callable_ = std::move(other.callable_);
    state_ = std::move(other.state_);
    handle_acquired_.store(handle_acquired_.load(std::memory_order_relaxed), std::memory_order_relaxed); 
  }

  ~task()
  {
    if (state_)
    {
      if (handle_acquired_.load(std::memory_order_relaxed) && !state_->is_ready())
      {
        state_->set_exception(std::make_exception_ptr(
              task_error{} << task_error_info{task_error_code::not_run} << EXCEPTION_LOCATION));
        state_->make_ready();
      }
    }
  }

  task& operator=(const task&) = delete;
  task& operator=(task&& other) = default;

  task_handle<result_type> get_handle() const
  {
    if (!state_)
      THROW(task_error{} << task_error_info{task_error_code::no_state});

    bool expected = false;
    // this could be memory_order_acq_rel operation, or maybe even relaxed?
    if (!handle_acquired_.compare_exchange_strong(expected, true))
      THROW(task_error{} << task_error_info{task_error_code::handle_already_acquired});

    return task_handle<result_type>{state_};
  }

  void operator()()
  {
    if (!state_)
      THROW(task_error{} << task_error_info{task_error_code::no_state});

    if (callable_) // is it necessary? yeap, sb. could pass empty std::function
    {
      if (!state_->mark_running())
      {
        // This should work as user can cancel only if task is in task_state::initialized state
        if (state_->is_cancelled()) return;
        THROW(task_error{} << task_error_info{task_error_code::already_called});
      }

      try
      {
        callable_();
        post_continuation();
      }
      catch (...)
      {
        state_->set_exception(std::current_exception());
      }

      state_->make_ready();
    }
  }

  template<typename F>
  task<typename std::result_of<F()>::type>& then(F&& action)
  {
    task<typename std::result_of<F()>::type> t{std::forward<F>(action)};
    return continuation_.set_task(std::move(t));
  }

private:
  void post_continuation()
  {
    if (continuation_.is_set())
      thread::current()->post_task(continuation_.move_out());
  }

  continuation continuation_;
  std::function<result_type()> callable_;
  std::shared_ptr<task_shared_state<result_type>> state_;
  mutable std::atomic<bool> handle_acquired_{false};
};

template<typename F, typename ...Args>
task<typename std::result_of<F(Args...)>::type> make_task(F&& f, Args&&... args)
{
  typedef typename std::result_of<F(Args...)>::type result_type;
  return task<result_type>{std::forward<F>(f), std::forward<Args>(args)...};
}

// TODO Move this to some task_helper header?
template<typename R>
task_handle<R> post_task_with_handle(thread& thread_,
                                      task<R>&& task_,
                                      time_delay delay = 0_ms)
{
  auto result = task_.get_handle();
  thread_.post_task(std::move(task_), delay);
  return result;
}
}

#endif
