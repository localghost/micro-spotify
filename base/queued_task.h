#ifndef BASE_QUEUED_TASK_H
#define BASE_QUEUED_TASK_H

#include <memory>
#include <utility>
#include <base/task.h>
#include <base/chrono.h>
#include <base/callable.h>

namespace base {
class queued_task
{
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
    : action(std::move(action)),
      when(w)
  { }

  friend bool operator<(const queued_task& x, const queued_task& y)
  {
    return x.when > y.when;
  }

  // TODO is it better than std::function<void()> action{[task_] { task_(); }}; ? 
  callable action;

  high_steady_clock::time_point when;
};
}

#endif
