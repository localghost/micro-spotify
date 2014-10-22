#ifndef BASE_THREAD_H
#define BASE_THREAD_H

#include <thread>
#include <chrono>
#include <functional>
#include <base/task.h>
#include <base/chrono.h>
#include <base/message_loop.h>
#include <base/waitable_event.h>

namespace base {
class thread
{
public:
    static thread* current();

    ~thread();

    void start(); // necessary except for completeness?

    // After this method is called this object can be re-used.
    // If start() is called now a new platform thread object is
    // created. It will have a different id().
    void stop();

    std::thread::id id() const;

    void queue_task(callable action, time_delay delay = 0_ms);

private:
    void exec();

    std::thread thread_;
    message_loop loop_;
    waitable_event waiter_;
};

template<typename R>
task_handle<R> queue_task_with_handle(thread& thread_,
                                      task<R>&& task_,
                                      time_delay delay = 0_ms)
{
  auto result = task_.get_handle();
  thread_.queue_task(std::move(task_), delay);
  return result;
}
}

#endif
