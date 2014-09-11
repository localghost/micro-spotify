#ifndef BASE_THREAD_H
#define BASE_THREAD_H

#include <thread>
#include <chrono>
#include <functional>
#include <base/task.h>
#include <base/message_loop.h>

namespace base {
class thread FINAL
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

    template<typename R>
    void queue_task(task<R>&& task_,
                    std::chrono::milliseconds delay = std::chrono::milliseconds{0})
    {
      assert(thread_.joinable());
      loop_.queue_task(std::move(task_), delay);
    }

private:
    void exec();

    std::thread thread_;
    message_loop loop_;
};

template<typename R>
task_handle<R> queue_task_with_handle(thread& thread_,
                                      task<R>&& task_,
                                      std::chrono::milliseconds delay = std::chrono::milliseconds{0})
{
  auto result = task_.get_handle();
  thread_.queue_task(std::move(task_), delay);
  return result;
}
}

#endif
