#include "message_loop.h"

namespace base {
message_loop::message_loop() : active_(false) { }

void message_loop::start()
{
  if (active_) return;

  active_ = true;

  exec();
}

void message_loop::stop()
{
// FIXME https://github.com/localghost/micro-spotify/wiki/Architecture#message-loop
//
//  active_ = false;

  queue_task(task{[this]() { active_ = false; }});
}

void message_loop::queue_task(task task_, std::chrono::milliseconds delay)
{
  // FIXME protect with mutex
  std::lock_guard<std::mutex> guard{queue_mutex_};
  queue_.emplace(std::move(task_), high_steady_clock::time_point{delay});
}

void message_loop::exec()
{
  // FIXME protect with mutex
  while (active_)
  {
    std::unique_lock<std::mutex> guard{queue_mutex_};
    queued_task t = queue_.top();
    queue_.pop();
    guard.unlock();
    t();
  }
}
}
