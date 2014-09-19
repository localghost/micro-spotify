#ifndef BASE_MESSAGE_LOOP_H
#define BASE_MESSAGE_LOOP_H

#include <vector>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <utility>
#include <base/task.h>
#include <base/queued_task.h>
#include <base/chrono.h>

namespace base {
// Tasks will be executed on the thread the start() method was invoked on.
class message_loop FINAL
{
public:
  void start();
  void stop();

  template<typename R>
  void queue_task(task<R>&& t)
  {
    queue_task_(queued_task{std::move(t)});
  }

  template<typename R>
  void queue_task(task<R>&& t, time_delay delay)
  {
    queue_task_(queued_task{std::move(t), high_steady_clock::now() + delay});
  }

private:
  void exec();

  void queue_task_(queued_task&& t);

  std::atomic<bool> active_{false};

  // TODO Rewrite to use lock-free priority queue
  std::vector<queued_task> queue_;
  std::mutex mutex_;
  std::condition_variable waiter_;

  high_steady_clock::time_point next_loop_time_;
};
}

#endif
