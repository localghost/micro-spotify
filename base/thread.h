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
    void stop();

    template<typename R>
    void queue_task(task<R> task_,
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
}

#endif
