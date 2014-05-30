#ifndef BASE_THREAD_H
#define BASE_THREAD_H

#include <thread>
#include <memory>
#include <chrono>
#include <base/task.h>
#include <base/message_loop.h>

namespace base {
class thread FINAL
{
public:
    static thread* current();

    thread();
    ~thread();

    void start(); // necessary except for completeness?
    void stop();

    void queue_task(task task_,
                    std::chrono::milliseconds delay = std::chrono::milliseconds{0});

private:
    void exec();

    std::unique_ptr<std::thread> thread_;
    message_loop loop_;
};
}

#endif
