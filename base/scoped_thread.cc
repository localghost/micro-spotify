#include "scoped_thread.h"

namespace base {
scoped_thread::scoped_thread(std::thread thr) : thread_(std::move(thr))
{
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
