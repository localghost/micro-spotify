#ifndef BASE_MESSAGE_LOOP_H
#define BASE_MESSAGE_LOOP_H

#include <vector>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <utility>
#include <base/queued_task.h>
#include <base/chrono.h>
#include <base/compatibility.h>
#include <base/export.h>

namespace base {
// Tasks will be executed on the thread the start() method was invoked on.
class EXPORT_API message_loop FINAL
{
public:
  message_loop() = default;
  message_loop(const message_loop&) = delete;
  message_loop& operator=(const message_loop&) = delete;

  void start();
  void stop();

  void post_task(callable action);
  void post_task(callable action, time_delay delay);

private:
  void exec();

  void post_queued_task(queued_task t);

  std::atomic<bool> active_{false};

  // TODO Rewrite to use lock-free priority queue
  std::vector<queued_task> queue_;
  std::mutex mutex_;
  std::condition_variable waiter_;

  high_steady_clock::time_point next_loop_time_;
};
}

#endif
