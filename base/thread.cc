#include "thread.h"

#include <cassert>

namespace base {
namespace {
thread_local thread* current_thread = nullptr;
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

  thread_.reset(new std::thread(std::bind(thread::exec, this)));
}

void thread::stop()
{
  if (!thread_) return;

  loop_.stop();
  thread_->join();
  thread_.reset();
}

void thread::queue_task(task task_)
{
  // FIXME Maybe return bool(false) when thread is not active
  //       and do not add task to the queue
  assert(thread_);
  loop_.queue_task(std::move(task_));
}

void thread::exec()
{
  current_thread = this;
  loop_.start();
}
}
