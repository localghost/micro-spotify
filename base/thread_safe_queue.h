#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <algorithm>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <base/queued_task.h>

namespace base {
// FIXME This is a very primitive version of a queue which uses one
//       mutex for the whole queue (so the lock contention is highly
//       possible). Strive for a lock-free queue in the first release.

class thread_safe_queue
{
public:
  // TODO utilise perfect fwd?
  void push(queued_task t)
  {
    {
      std::lock_guard<std::mutex> guard{lock_};
      queue_.push(std::move(t));
      std::push_heap(queue_.begin(), queue_.end());
    }
    cv_.notify_one();
  }

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
  std::vector<queued_task> queue_;
  std::mutex lock_;
  std::condition_variable cv_;
};
}

#endif
