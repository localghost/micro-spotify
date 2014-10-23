#ifndef BASE_THREAD_H
#define BASE_THREAD_H

#include <thread>
#include <utility>
#include <base/callable.h>
#include <base/chrono.h>
#include <base/message_loop.h>
#include <base/waitable_event.h>
#include <base/export.h>

namespace base {
class EXPORT_API thread
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

    void post_task(callable action, time_delay delay = 0_ms);

private:
    void exec();

    std::thread thread_;
    message_loop loop_;
    waitable_event waiter_;
};
}

#endif
