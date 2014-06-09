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

thread::thread() : thread_(nullptr) { }

thread::~thread()
{
  assert(!thread_);
}

void thread::start()
{
  // FIXME NOT thread-safe
  if (thread_) return;

  thread_.reset(new std::thread{&thread::exec, this});
}

void thread::stop()
{
  if (!thread_) return;

  loop_.stop();
  thread_->join();
  thread_.reset();
}

void thread::queue_task(task task_,
                        std::chrono::milliseconds delay)
{
  // FIXME Maybe return bool(false) when thread is not active
  //       and do not add task to the queue
  assert(thread_);
  loop_.queue_task(std::move(task_), delay);
}

void thread::exec()
{
  current_thread = this;
  loop_.start();
}
}
