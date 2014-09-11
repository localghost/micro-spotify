#ifndef BASE_WAITABLE_EVENT_H
#define BASE_WAITABLE_EVENT_H

#include <mutex>
#include <condition_variable>

// TODO Add support for notify_all(), etc...
// FIXME It supports only one thread
class waitable_event
{
public:
  void wait()
  {
    std::unique_lock<std::mutex> guard{m_};
    signaled_ = false;
    cv_.wait(guard, [&]{ return signaled_; });
  }

  void signal()
  {
    {
      std::lock_guard<std::mutex> guard{m_};
      signaled_ = true;
    }
    cv_.notify_one();
  }
private:
  std::mutex m_;
  std::condition_variable cv_;
  bool signaled_ = false;
};

#endif
