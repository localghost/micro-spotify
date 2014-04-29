#ifndef BASE_MESSAGE_LOOP_H
#define BASE_MESSAGE_LOOP_H

#include <condition_variable>
#include <base/task.h>
#include <base/thread_safe_queue.h>

namespace base {
// Tasks will be executed on the thread the start() method was invoked on.
class message_loop FINAL
{
  struct queued_task
  {
    high_steady_clock::time_point when_;
    task task_;
  };
public:
  message_loop();

  void start();
  void stop();

  void queue_task(task task_);

private:
  void exec();

//  std::atomic<bool> active_;
  bool active_;
  thread_safe_queue<task> queue_;
  std::condition_variable waiter_;
};
}

#endif
