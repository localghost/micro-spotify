#ifndef BASE_MESSAGE_LOOP_H
#define BASE_MESSAGE_LOOP_H

#include <queue>
#include <condition_variable>
#include <mutex>
#include <base/task.h>
#include <base/traits.h>

namespace base {
// Tasks will be executed on the thread the start() method was invoked on.
class message_loop FINAL
{
private:
  struct queued_task
  {
    task task_;
    high_steady_clock::time_point when_;

    friend bool operator<(const queued_task& x, const queued_task& y) const
    {
      return x.when_ < y.when_;
    }
  };

public:
  message_loop();

  void start();
  void stop();

  void queue_task(task task_,
                  std::chrono::milliseconds delay = 0);

private:
  void exec();

//  std::atomic<bool> active_;
  bool active_;

  // TODO Rewrite to use lock-free priority queue
  std::priority_queue<queued_task> queue_;
  std::mutex queue_mutex_;

  std::condition_variable waiter_;
  std::mutex waiter_mutex_;
};
}

#endif
