#ifndef BASE_THREAD_H
#define BASE_THREAD_H

#include <thread>
#include <base/task.h>
#include <base/thread_safe_queue.h>

namespace base {
class thread FINAL
{
public:
    static thread& current();

    thread();
    ~thread();

    void start(); // necessary except for completeness?
    void quit();

    void post_task(task task_);

private:
    void exec();

    std::thread* thread_;
    thread_safe_queue<task> queue_;
};
}

#endif
