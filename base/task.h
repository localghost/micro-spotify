#ifndef BASE_TASK_H
#define BASE_TASK_H

#include <utility>
#include <memory>
#include <atomic>
#include <functional>
#include <exception>
#include <base/exception.h>
#include <base/compatibility.h>

namespace base {
template<typename> class task;

enum struct task_error_code
{
  no_state,
  cancelled,
  not_run
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
    return *static_cast<const result_type*>(static_cast<const void*>(&result_));
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

  bool is_ready() const
  {
    return (state_ == state::cancelled || state_ == state::finished);
  }

  void make_ready()
  {
    state_ = state::finished;
    wait_cv_.notify_all();
  }

  bool mark_running()
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

  bool cancel()
  {
    state expected = state::initialized;
    return state_.compare_exchange_strong(expected, state::cancelled);
  }

  bool is_cancelled() const
  {
    return state::cancelled == state_;
  }

//  task_state::state get_state() const
//  {
//    return state_;
//  }

  void set_exception(std::exception_ptr exception)
  {
    exception_ = exception;
  }

  std::exception_ptr get_exception() const
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

  result_type get() const
  {
    wait();

    // FIXME
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

  bool cancel()
  {
    if (!state_)
      THROW(task_error{} << task_error_info{task_error_code::no_state});

    return state_->cancel();
  }

private:
  template<typename> friend class task;

  explicit task_handle(const std::shared_ptr<task_state<result_type>>& state) : state_(state) { }

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

  result_type get() const
  {
    wait();

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

private:
  template<typename> friend class task;

  explicit task_handle(const std::shared_ptr<task_state<result_type>>& state) : state_(state) { }

  std::shared_ptr<task_state<result_type>> state_;
};

template<typename R>
class task FINAL
{
public:
  typedef R result_type;

  task() = default;

  // TODO Accepting only callable enforces to use std::bind() if arguments to the callable
  //      should be passed. Should this ctor accept also arguments, similarly to e.g. std::packaged_task??
  //      Should it be protected against too perfect fwd? (guess so, it could produce some weird errors)
  // This looks like a premature optimization, is it? Isn't std::bind() optimized already for callables
  // that don't take any arguments?
  template<typename F>
  explicit task(F&& callable)
    : callable_(std::forward<F>(callable)),
      state_(new task_state<result_type>)
  { }

  // TODO Accepting only callable enforces to use std::bind() if arguments to the callable
  //      should be passed. Should this ctor accept also arguments, similarly to e.g. std::packaged_task??
  template<typename F, typename ...Args>
  explicit task(F&& callable, Args&&... args)
    : callable_(std::bind(std::forward<F>(callable), std::forward<Args>(args)...)),
      state_(new task_state<result_type>)
  { }

  task(const task&) = delete;
  task(task&& other) = default;

  // FIXME and copy to task<void>
  ~task()
  {
    if (state_)
    {
      if (handle_acquired && !state_->is_ready())
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
    // FIXME if this function is called but task is not invoked then in dtor set exception
    //       in the shared state (task_error_code::not_run)

    if (!state_)
      THROW(task_error{} << task_error_info{task_error_code::no_state});

    handle_acquired = true;

    return task_handle<result_type>{state_};
  }

  void operator()()
  {
    if (!state_)
      THROW(task_error{} << task_error_info{task_error_code::no_state});

    if (callable_)
    {
      // FIXME differentiate between cancelled and already running
      //       and in case of the latter one throw exception
      if (!state_->mark_running()) return;
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
  mutable bool handle_acquired = false; // does it need to be atomic? does it need to be mutable (should get_handle() be const)?
};

template<>
class task<void> FINAL
{
public:
  typedef void result_type;

  task() = default;

  template<typename F>
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
  task(task&& other) = default;

  ~task()
  {
    if (state_)
    {
      if (handle_acquired && !state_->is_ready())
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

    handle_acquired = true;

    return task_handle<result_type>{state_};
  }

  void operator()()
  {
    if (!state_)
      THROW(task_error{} << task_error_info{task_error_code::no_state});

    if (callable_) // is it necessary? yeap, sb. could pass empty std::function
    {
      if (!state_->mark_running()) return;
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
  mutable bool handle_acquired = false;
};
}

#endif
