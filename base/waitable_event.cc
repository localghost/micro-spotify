#include "waitable_event.h"

namespace base {
void waitable_event::wait()
{
  std::unique_lock<std::mutex> guard{m_};
  cv_.wait(guard, [&]{ return signaled_; });
}

void waitable_event::signal()
{
  {
    std::lock_guard<std::mutex> guard{m_};
    signaled_ = true;
  }
  cv_.notify_all();
}
}
