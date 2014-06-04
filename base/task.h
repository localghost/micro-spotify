#ifndef BASE_TASK_H
#define BASE_TASK_H

#include <utility>
#include <base/callable.h>

namespace base {
class task FINAL
{
public:
  task() = default;

  explicit task(callable callable_);

  task(const task&) = delete;

  task(task&& other);

  task& operator=(const task&) = delete;

  task& operator=(task&& other);

  void operator()();

private:
  callable callable_;
};
}

#endif
