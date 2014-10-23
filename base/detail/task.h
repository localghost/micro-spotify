#ifndef BASE_DETAIL_TASK_H
#define BASE_DETAIL_TASK_H

#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <exception>
#include <utility>
#include <boost/scope_exit.hpp>
#include <base/assert.h>
#include <base/exception.h>
#include <base/callable.h>

namespace base {
namespace detail {
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

class continuation
{
private:
  struct base
  {
    virtual ~base() {}
    virtual callable move_task() = 0;
  };

  template<typename T>
  struct model : base
  {
    explicit model(T t) : callable_{std::move(t)} {}

    callable move_task()
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

  callable move_task()
  {
    BOOST_SCOPE_EXIT_ALL(this) { ptr_.reset(); };
    return ptr_->move_task();
  }

  bool is_set() const
  {
    return ptr_.get() != nullptr;
  }

private:
  std::unique_ptr<base> ptr_;
};
} 
}

#endif
