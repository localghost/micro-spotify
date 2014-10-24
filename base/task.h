#ifndef BASE_TASK_H
#define BASE_TASK_H

#include <memory>
#include <functional>
#include <exception>
#include <type_traits>
#include <utility>
#include <boost/scope_exit.hpp>
#include <base/exception.h>
#include <base/thread.h>
#include <base/export.h>
#include <base/detail/task.h>

namespace base {
enum struct EXPORT_API task_error_code
{
  no_state,
  cancelled,
  not_run,
  already_called,
  handle_already_acquired
};
typedef ::boost::error_info<struct EXPORT_API task_error_code_tag, task_error_code> task_error_info;
EXCEPTION_TYPE(task_error);

template<typename R>
class EXPORT_API task_handle FINAL
{
public:
  typedef R result_type;

  task_handle() = default;
  task_handle(const task_handle&) = delete;
  task_handle(task_handle&&) noexcept = default;

  void operator=(const task_handle&) = delete;
  task_handle& operator=(task_handle&&) noexcept = default;

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

  explicit task_handle(const std::shared_ptr<detail::task_shared_state<result_type>>& state) noexcept
    : state_(state) { }

  std::shared_ptr<detail::task_shared_state<result_type>> state_;
};

template<typename R>
class EXPORT_API task FINAL
{
public:
  typedef R result_type;

  task() = default;

  // This looks like a premature optimization, is it? Isn't std::bind() optimized already for callables
  // that don't take any arguments?
  template<typename F, typename std::enable_if<!std::is_same<task, typename std::decay<F>::type>::value>::type* = nullptr>
  explicit task(F&& callable)
    : callable_(std::forward<F>(callable)),
      state_(new detail::task_shared_state<result_type>)
  { }

  template<typename F, typename ...Args>
  explicit task(F&& callable, Args&&... args)
    : callable_(std::bind(std::forward<F>(callable), std::forward<Args>(args)...)),
      state_(new detail::task_shared_state<result_type>)
  { }

  task(const task&) = delete;
  task(task&& other) = default;

  ~task()
  {
    if (state_)
    {
      if (handle_acquired_ && !state_->is_ready())
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

    if (handle_acquired_)
      THROW(task_error{} << task_error_info{task_error_code::handle_already_acquired});
    handle_acquired_ = true;

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
      thread::current()->post_task(continuation_.move_task());
  }

  detail::continuation continuation_;
  std::function<result_type()> callable_;
  std::shared_ptr<detail::task_shared_state<result_type>> state_;
  mutable bool handle_acquired_{false};
};

template<>
class EXPORT_API task<void> FINAL
{
public:
  typedef void result_type;

  task() = default;

  template<typename F, typename std::enable_if<!std::is_same<task, typename std::decay<F>::type>::value>::type* = nullptr>
  explicit task(F&& callable)
    : callable_(std::forward<F>(callable)),
      state_(new detail::task_shared_state<result_type>)
  { }

  template<typename F, typename ...Args>
  explicit task(F&& callable, Args&&... args) 
    : callable_(std::bind(std::forward<F>(callable), std::forward<Args>(args)...)),
      state_(new detail::task_shared_state<result_type>)
  { }

  task(const task&) = delete;
  task(task&& other) = default;

  ~task()
  {
    if (state_)
    {
      if (handle_acquired_ && !state_->is_ready())
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

    // this could be memory_order_acq_rel operation, or maybe even relaxed?
    if (handle_acquired_)
      THROW(task_error{} << task_error_info{task_error_code::handle_already_acquired});
    handle_acquired_ = true;

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
      thread::current()->post_task(continuation_.move_task());
  }

  detail::continuation continuation_;
  std::function<result_type()> callable_;
  std::shared_ptr<detail::task_shared_state<result_type>> state_;
  mutable bool handle_acquired_{false};
};

template<typename F, typename ...Args>
task<typename std::result_of<F(Args...)>::type> make_task(F&& f, Args&&... args) EXPORT_API;
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
                                      time_delay delay = 0_ms) EXPORT_API;
template<typename R>
task_handle<R> post_task_with_handle(thread& thread_,
                                      task<R>&& task_,
                                      time_delay delay)
{
  auto result = task_.get_handle();
  thread_.post_task(std::move(task_), delay);
  return result;
}
}

#endif
