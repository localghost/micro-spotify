#ifndef BASE_TASK_H
#define BASE_TASK_H

#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <exception>
#include <type_traits>
#include <boost/scope_exit.hpp>
#include <base/exception.h>
#include <base/compatibility.h>

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

template<typename R>
class task_state_result_storage
{
public:
  typedef R result_type;

  template<typename T>
  void set_result(T&& result)
  {
    ::new (&result_) result_type{std::forward<T>(result)};
  }

  result_type get_result() const
  {
    return std::move(*static_cast<const result_type*>(static_cast<const void*>(&result_)));
  }

private:
  typename std::aligned_storage<sizeof(result_type), alignof(result_type)>::type result_;
};

template<>
class task_state_result_storage<void>
{
  typedef void result_type;
};

template<typename R>
class task_state : public task_state_result_storage<R>
{
public:
  enum struct state { initialized, running, cancelled, finished };

  task_state() : state_{state::initialized} {}

  bool is_ready() const noexcept
  {
    return (state_ == state::cancelled || state_ == state::finished);
  }

  void make_ready() noexcept
  {
    state_ = state::finished;
    wait_cv_.notify_all();
  }

  bool mark_running() noexcept
  {
    state expected = state::initialized;
    return state_.compare_exchange_strong(expected, state::running);
  }

  void wait() const
  {
    if (is_ready()) // premature optimization?
      return;

    std::unique_lock<std::mutex> lock(wait_mutex_);
    wait_cv_.wait(lock, std::bind(&task_state::is_ready, this));
  }

  bool cancel() noexcept
  {
    state expected = state::initialized;
    return state_.compare_exchange_strong(expected, state::cancelled);
  }

  bool is_cancelled() const noexcept
  {
    return state::cancelled == state_;
  }

//  task_state::state get_state() const
//  {
//    return state_;
//  }

  void set_exception(std::exception_ptr exception) noexcept
  {
    exception_ = exception;
  }

  std::exception_ptr get_exception() const noexcept
  {
    return exception_;
  }

private:
  std::atomic<state> state_;
  mutable std::mutex wait_mutex_;
  mutable std::condition_variable wait_cv_;
  std::exception_ptr exception_;
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

  explicit task_handle(const std::shared_ptr<task_state<result_type>>& state) noexcept
    : state_(state) { }

  std::shared_ptr<task_state<result_type>> state_;
};

template<>
class task_handle<void> FINAL
{
public:
  typedef void result_type;

  task_handle() = default;
  task_handle(const task_handle&) = delete;
  task_handle(task_handle&&) = default;

  void operator=(const task_handle&) = delete;
  task_handle& operator=(task_handle&&) = default;

  result_type get()
  {
    wait();

    BOOST_SCOPE_EXIT_ALL(this) { state_.reset(); };

    // FIXME
    if (state_->is_cancelled())
      THROW(task_error{} << task_error_info{task_error_code::cancelled});

    std::exception_ptr exception = state_->get_exception();
    if (nullptr != exception)
      std::rethrow_exception(exception);
  }

  void wait() const
  {
    if (!state_)
      THROW(task_error{} << task_error_info{task_error_code::no_state});

    state_->wait();
  }

  bool cancel()
  {
    if (!state_)
      THROW(task_error{} << task_error_info{task_error_code::no_state});

    return state_->cancel();
  }

  bool is_valid() const
  {
    return bool(state_);
  }

private:
  template<typename> friend class task;

  explicit task_handle(const std::shared_ptr<task_state<result_type>>& state) noexcept
    : state_(state) { }

  std::shared_ptr<task_state<result_type>> state_;
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
      state_(new task_state<result_type>)
  { }

  template<typename F, typename ...Args>
  explicit task(F&& callable, Args&&... args)
    : callable_(std::bind(std::forward<F>(callable), std::forward<Args>(args)...)),
      state_(new task_state<result_type>)
  { }

  task(const task&) = delete;
  task(task&& other)
  {
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
      }
      catch (...)
      {
          state_->set_exception(std::current_exception());
      }

      state_->make_ready();
    }
  }

private:
  std::function<result_type()> callable_;
  std::shared_ptr<task_state<result_type>> state_;
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
      state_(new task_state<result_type>)
  { }

  template<typename F, typename ...Args>
  explicit task(F&& callable, Args&&... args) 
    : callable_(std::bind(std::forward<F>(callable), std::forward<Args>(args)...)),
      state_(new task_state<result_type>)
  { }

  task(const task&) = delete;
  task(task&& other)
  {
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
      }
      catch (...)
      {
        state_->set_exception(std::current_exception());
      }

      state_->make_ready();
    }
  }

private:
  std::function<result_type()> callable_;
  std::shared_ptr<task_state<result_type>> state_;
  mutable std::atomic<bool> handle_acquired_{false};
};

template<typename F, typename ...Args>
task<typename std::result_of<F(Args...)>::type> make_task(F&& f, Args&&... args)
{
  typedef typename std::result_of<F(Args...)>::type result_type;
  return task<result_type>{std::forward<F>(f), std::forward<Args>(args)...};
}
}

#endif
