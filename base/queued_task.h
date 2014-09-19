#ifndef BASE_QUEUED_TASK_H
#define BASE_QUEUED_TASK_H

#include <memory>
#include <utility>
#include <base/task.h>
#include <base/chrono.h>

namespace base {
class queued_task
{
private:
  struct task_model_base
  {
    virtual ~task_model_base() = default;
    virtual void call() = 0;
  };

  template<typename R>
  class task_model : public task_model_base
  {
  public:
    explicit task_model(task<R>&& action) : action(std::move(action)) {}

    void call()
    {
      action();
    }

  private:
    task<R> action;
  };

public:
  // TODO maybe set when to default (epoch); if high_steady_clock mallfunctioned
  //      this would prevent current tasks to be delayed? but on the other hand
  //      if someone pushes task with when set to now() and the default then
  //      the latter one would preceed the first (client might not expect that)
  template<typename R>
  explicit queued_task(task<R>&& action)
    : queued_task(std::move(action), high_steady_clock::now())
  { }

  template<typename R>
  queued_task(task<R>&& action, high_steady_clock::time_point w)
    : action(new task_model<R>{std::move(action)}),
      when(w)
  { }

  friend bool operator<(const queued_task& x, const queued_task& y)
  {
    return x.when > y.when;
  }

  std::unique_ptr<task_model_base> action;

  high_steady_clock::time_point when;
};
}

#endif
