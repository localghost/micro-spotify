#include "scoped_thread.h"

#include "assert.h""

namespace base {
scoped_thread::scoped_thread(std::thread t) : thread_(std::move(t))
{
	PRECONDITION(t.joinable())
}

scoped_thread::~scoped_thread()
{
  // the thread passed to scoped_thread must be joinable
  thread_.join();
}

const std::thread& scoped_thread::operator*() const
{
  return thread_;
}

const std::thread* scoped_thread::operator->() const
{
  return &thread_;
}
} // namespace base
