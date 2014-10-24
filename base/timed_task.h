#ifndef BASE_TIMED_TASK_H
#define BASE_TIMED_TASK_H

#include <memory>
#include <utility>
#include <base/chrono.h>
#include <base/callable.h>

namespace base {
class timed_task
{
public:
  // TODO maybe set when to default (epoch); if high_steady_clock mallfunctioned
  //      this would prevent current tasks to be delayed? but on the other hand
  //      if someone pushes task with when set to now() and the default then
  //      the latter one would preceed the first (client might not expect that)
  explicit timed_task(callable action)
    : timed_task(std::move(action), high_steady_clock::now())
  { }

  timed_task(callable action, high_steady_clock::time_point w)
    : action(std::move(action)),
      when(w)
  { }

  timed_task(const timed_task&) noexcept = default;
  timed_task(timed_task&&) noexcept = default;

  timed_task& operator=(const timed_task&) noexcept = default;
  timed_task& operator=(timed_task&&) noexcept = default;

  friend bool operator<(const timed_task& x, const timed_task& y)
  {
    return x.when > y.when;
  }

  // TODO is it better than std::function<void()> action{[task_] { task_(); }}; ? 
  callable action;

  high_steady_clock::time_point when;
};
}

#endif
