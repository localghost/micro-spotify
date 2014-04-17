#ifndef MESSAGE_LOOP_H
#define MESSAGE_LOOP_H

#include <base/thread_safe_queue.h>

namespace base {
class message_loop FINAL
{
public:
  message_loop();

  void start();
  void stop();

  void queue_task(task task_);

private:
  void exec();

  std::atomic<bool> active_;
  thread_safe_queue<task> queue_;
};
}

#endif
