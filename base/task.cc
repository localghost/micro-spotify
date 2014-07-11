#include "task.h"

namespace base {
task::task(callable callable_) : callable_(std::move(callable_)) { }

task::task(task&& other)
{
  // TODO implement me!
  callable_ = std::move(other.callable_);
}

task& task::operator=(task&& other)
{
  // TODO implement me!
  callable_ = std::move(other.callable_);
  return *this;
}

void task::operator()()
{
  try
  {
    callable_();
    waiter_.notify_all();
  }
  catch (...)
  {
    // TODO set caught exception
    waiter_.notify_all();
  }
}
}
