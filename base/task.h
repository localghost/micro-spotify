#ifndef BASE_TASK_H
#define BASE_TASK_H

#include <base/callable.h>

namespace base {
class task
{
public:
  task();

  explicit task(callable callable_) : callable_(std::move(callable_)) { }

  void run()
  {
    callable_();
  }

private:
  callable callable_;
};
}

#endif
