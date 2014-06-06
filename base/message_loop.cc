#include "message_loop.h"

#include <algorithm>

#include "log.h"

namespace base {
message_loop::message_loop() : active_(false) { }

void message_loop::start()
{
  if (active_) return;

  active_ = true;

  exec();
}

void message_loop::stop()
{
// FIXME https://github.com/localghost/micro-spotify/wiki/Architecture#message-loop
//
//  active_ = false;

  queue_task(task{[this]() { active_ = false; }});
}

void message_loop::queue_task(task task_, std::chrono::milliseconds delay)
{
  // FIXME see https://github.com/localghost/micro-spotify/wiki/Architecture:-MessageLoop#adding-task-to-not-running-message-loop 
  bool notify_waiter = false;
  {
    std::lock_guard<std::mutex> guard{mutex_};
    LOG_DEBUG << "putting task";

    bool is_empty = queue_.empty();

    queue_.push_back(queued_task{std::move(task_), high_steady_clock::now() + delay});
    std::push_heap(queue_.begin(), queue_.end());

    if (is_empty || (queue_.front().when < next_loop_time_))
    {
      next_loop_time_ = queue_.front().when;
      notify_waiter = true;
    }
  }
  if (notify_waiter)
    waiter_.notify_one();
}

void message_loop::exec()
{
  while (active_)
  {
    std::vector<task> tasks;
    {
      std::unique_lock<std::mutex> guard{mutex_};

      // FIXME Check if behaviour for wait_until() with time point in the past
      //       is defined. If so, and it behaves like regular wait(), then
      //       the code in else clause alone is sufficient
      if (queue_.empty())
        waiter_.wait(guard, [this]{ return queue_.empty(); });
      else
        waiter_.wait_until(guard, next_loop_time_, [this]{ return queue_.empty(); });

      while (!queue_.empty() && (queue_.front().when <= high_steady_clock::now()))
      {
        std::pop_heap(queue_.begin(), queue_.end());
        tasks.push_back(std::move(queue_.back().task_));
        queue_.pop_back();
      }

      if (!queue_.empty()) next_loop_time_ = queue_.front().when;
    }    
    for (task& t : tasks)
      t();
  }
}
}
