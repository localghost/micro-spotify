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
  active_ = false;
  // FIXME Push task to the queue so that waiting threads
  //       is resumed
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
