#include "task_queue.h"

#include <algorithm>
#include <base/chrono.h>

namespace base {
void task_queue::push(timed_task t)
{
  bool notify = false;
  {
    std::lock_guard<std::mutex> guard{lock_};
    queue_.push_back(std::move(t));
    std::push_heap(queue_.begin(), queue_.end());
    notify = queue_.front().when <= high_steady_clock::now();
  }
  if (notify)
    cv_.notify_one();
}

std::vector<timed_task> task_queue::wait_and_pop()
{
  std::vector<timed_task> result;
  {
    std::unique_lock<std::mutex> guard{lock_};
    // FIXME Check if behaviour for wait_until() with time point in the past
    //       is defined. If so, and it behaves like regular wait(), then
    //       the code in else clause alone is sufficient
    if (queue_.empty())
      cv_.wait(guard, [this]() { return queue_.empty(); });
    else
      cv_.wait_until(guard, queue_.front().when, [this]() { return queue_.empty(); });
    const auto now = high_steady_clock::now();
    while (!queue_.empty() && (queue_.front().when <= now))
    {
      std::pop_heap(queue_.begin(), queue_.end());
      result.push_back(std::move(queue_.back()));
      queue_.pop_back();
    }
  }
  return result;
}

void task_queue::clear()
{
  std::lock_guard<std::mutex> guard{lock_};
  queue_.clear();
}
}
