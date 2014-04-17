#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <mutex>
#include <condition_variable>
#include <queue>
#include <base/task.h>

namespace base {
// FIXME This is a very primitive version of a queue which uses one
//       mutex for the whole queue (so the lock contention is highly
//       possible). Strive for a lock-free queue in the first release.

// Requirements: T is MoveConstructible or CopyConstructible
template <typename T>
class thread_safe_queue
{
public:
  // FIXME Might be necessary to allow conversions  
  template<typename U,
           typename std::enable_if<std::is_same<T, U>::value>::type* = nullptr>
  void push(U&& value)
  {
    {
      std::lock_guard<std::mutex> guard{lock_};
      queue_.push(std::forward<U>(value));
    }
    cv_.notify_one();
  }

//  void push(T&& value)
//  {
//    {
//      std::lock_guard<std::mutex> guard{lock_};
//      queue_.push(std::move(value));
//    }
//    cv_.notify_one();
//  }

  void pop(T& result)
  {
    std::unique_lock<std::mutex> guard{lock_};
    cv_.wait(guard, [this]() { return !queue_.empty(); });
    // FIXME If the move throws no other threads will be notified
    //       and the stored value will stay in the queue forever;
    //       using notify_all() might help with that but it is more
    //       expensive
    result = std::move(queue_.front());
    queue_.pop();
  }
  
  bool try_pop(T& result)
  {
      std::lock_guard<std::mutex> guard{lock_};
      
      if (queue_.empty()) return false;

      result = std::move(queue_.front());
      queue_.pop();

      return true;
  }

private:
  std::queue<T> queue_;
  std::mutex lock_;
  std::condition_variable cv_;
};
}

#endif
