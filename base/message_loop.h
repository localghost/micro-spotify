#ifndef BASE_MESSAGE_LOOP_H
#define BASE_MESSAGE_LOOP_H

#include <atomic>
#include <base/timed_task.h>
#include <base/chrono.h>
#include <base/compatibility.h>
#include <base/export.h>
#include <base/task_queue.h>

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

  void post_timed_task(timed_task t);

  std::atomic<bool> active_{false};

  task_queue queue_;
};
}

#endif
