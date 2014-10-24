#ifndef BASE_WAITABLE_EVENT_H
#define BASE_WAITABLE_EVENT_H

#include <mutex>
#include <condition_variable>
#include <base/export.h>

namespace base {
/// This is a primitive synchronisation mechanism between two threads.
/// One threads waits on it to be signaled while the other one may signal
/// it at any given time.
class EXPORT_API waitable_event
{
public:
  void wait();
  void wait_and_reset();

  /// @warning Only one thread is signaled
  void signal();

private:
  void wait_on_lock(std::unique_lock<std::mutex>& lock);

  std::mutex m_;
  std::condition_variable cv_;
  bool signaled_ = false;
};
}

#endif
