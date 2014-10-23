#include "message_loop.h"

#include <algorithm>
#include <boost/exception/all.hpp>

#include "log.h"
#include "thread.h"

namespace base {
void message_loop::start()
{
  if (active_) return;

  active_ = true;

  exec();
}

// FIXME this method should only set active_ to false but there should be other
//       method for clearing the message loop (or stop_and_clear()), or maybe stop
//       should be synchronous
void message_loop::stop()
{
// FIXME https://github.com/localghost/micro-spotify/wiki/Architecture#message-loop
//
//  active_ = false;

  post_task([this]
        {
          {
            std::lock_guard<std::mutex> guard{mutex_};
            queue_.clear();
          }
          active_ = false;
        });
}

void message_loop::post_task(callable action)
{
  post_queued_task(queued_task{std::move(action)});
}

void message_loop::post_task(callable action, time_delay delay)
{
  post_queued_task(queued_task{std::move(action), high_steady_clock::now() + delay});
}

// FIXME Consider running a task directly if it was added on the same thread
//       on which message_loop is spinning and it has no delay; use case: 
//       task is pushed on a loop and get() on its handle is called, in such
//       case application blocks
void message_loop::post_queued_task(queued_task t)
{
//  if (!active_) return;
//  LOG_DEBUG << "pushing a task on a thread: " << base::thread::current()->id();

  // FIXME see https://github.com/localghost/micro-spotify/wiki/Architecture:-MessageLoop#adding-task-to-not-running-message-loop 
  bool notify_waiter = false;
  {
    std::lock_guard<std::mutex> guard{mutex_};

    bool is_empty = queue_.empty();

    queue_.push_back(std::move(t));
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

// TODO Add exception-safety
void message_loop::exec()
{
  while (active_)
  {
    std::vector<queued_task> tasks;
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
        tasks.push_back(std::move(queue_.back()));
        queue_.pop_back();
      }

      if (!queue_.empty()) next_loop_time_ = queue_.front().when;
    }    
    for (queued_task& t : tasks)
      t.action();
  }
}
}
