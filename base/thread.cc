#include "thread.h"

#include <cassert>

#include "compatibility.h"

namespace base {
namespace {
THREAD_LOCAL thread* current_thread = nullptr;
}

thread* thread::current()
{
  return current_thread;
}

thread::~thread()
{
  assert(!thread_.joinable());
}

void thread::start()
{
  // FIXME NOT thread-safe
  if (thread_.joinable()) return;

  thread_ = std::thread{&thread::exec, this};
}

void thread::stop()
{
  if (!thread_.joinable()) return;

  loop_.stop();
  thread_.join();
}

std::thread::id thread::id() const
{
  return thread_.get_id();
}

void thread::exec()
{
  current_thread = this;
  loop_.start();
}
}
