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

void message_loop::queue_task(task task_)
{
  queue_.push(std::move(task_));
}

void message_loop::exec()
{
  while (active_)
  {
    task t = queue_.pop();
    t();
  }
}
}
