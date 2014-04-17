#include "thread.h"

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
  stop();
}

void thread::start()
{
  // FIXME NOT thread-safe
  if (thread_) return;

  thread_ = new std::thread(std::bind(thread::exec, this));
}

void thread::stop()
{
  if (!thread_) return;

  // This causes the loop to stop at next iteration
  // if first draining the loop is preferred queue the task
  // which will set active_ to false in the thread proc
  loop_.stop();
  thread_->join();
  delete thread_;
  thread_ = nullptr;
}

void thread::queue_task(task task_)
{
  loop_.queue_task(std::move(task_));
}

void thread::exec()
{
  loop_.start();
}
}
