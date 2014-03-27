#ifndef BASE_THREAD_H
#define BASE_THREAD_H

#include <thread>
#include <base/callable.h>
#include <base/thread_safe_queue.h>

namespace base {
class thread FINAL
{
public:
    thread();
    ~thread();

    void join();

private:
    void run();

    thread_safe_queue<callable> queue_;
};
}

#endif
