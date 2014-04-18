#ifndef BASE_TASK_H
#define BASE_TASK_H

#include <utility>
#include <base/callable.h>

namespace base {
class task FINAL
{
public:
  task() = default;

  explicit task(callable callable_) : callable_(std::move(callable_)) { }

  void operator()()
  {
    callable_();
  }

private:
  callable callable_;
};
}

#endif
