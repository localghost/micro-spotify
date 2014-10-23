#ifndef BASE_WAITABLE_EVENT_H
#define BASE_WAITABLE_EVENT_H

#include <mutex>
#include <condition_variable>
#include <base/export.h>

namespace base {
class EXPORT_API waitable_event
{
public:
  void wait();
  void signal();

private:
  std::mutex m_;
  std::condition_variable cv_;
  bool signaled_ = false;
};
}

#endif
