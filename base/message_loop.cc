#include "message_loop.h"

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

  queue_task(task([this]() { active_ = false; }));
}

void message_loop::queue_task(task task_, std::chrono::milliseconds delay)
{
  bool notify_waiter = false;
  {
    std::lock_guard<std::mutex> guard{queue_mutex_};

    if (queue_.empty()) notify_waiter = true;

    queue_.push(queued_task{std::move(task_), high_steady_clock::time_point{delay}});

    if (queue_.top().when < next_loop_time_)
    {
      next_loop_time_ = queue_.top().when;
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
      std::unique_lock<std::mutex> guard{queue_mutex_};

      if (queue_.empty())
        waiter_.wait(guard, [this](){ return queue_.empty(); });
      else
        waiter_.wait_until(guard, next_loop_time_, [this](){ return queue_.empty(); });

      while (!queue_.empty() && (queue_.top().when <= high_steady_clock::now()))
      {
        tasks.push_back(std::move(queue_.top().task_));
        queue_.pop();
      }

      if (!queue_.empty()) next_loop_time_ = queue_.top().when;
    }    
    for (task& t : tasks)
      t();
  }
}
}
