#ifndef BASE_MESSAGE_LOOP_H
#define BASE_MESSAGE_LOOP_H

#include <vector>
#include <condition_variable>
#include <mutex>
#include <base/task.h>
#include <base/chrono.h>

namespace base {
// Tasks will be executed on the thread the start() method was invoked on.
class message_loop FINAL
{
private:
  struct queued_task
  {
    task task_;
    high_steady_clock::time_point when;

    friend bool operator<(const queued_task& x, const queued_task& y)
    {
      return x.when > y.when;
    }
  };

public:
  message_loop();

  void start();
  void stop();

  void queue_task(task task_,
                  std::chrono::milliseconds delay = std::chrono::milliseconds{0});

private:
  void exec();

  bool active_;

  // TODO Rewrite to use lock-free priority queue
  std::vector<queued_task> queue_;
  std::mutex mutex_;
  std::condition_variable waiter_;

  high_steady_clock::time_point next_loop_time_;
};
}

#endif
