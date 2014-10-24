#include "message_loop.h"

#include <utility>
#include <boost/exception/all.hpp>

#include "log.h"

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
            queue_.clear();
          }
          active_ = false;
        });
}

void message_loop::post_task(callable action)
{
  post_timed_task(timed_task{std::move(action)});
}

void message_loop::post_task(callable action, time_delay delay)
{
  post_timed_task(timed_task{std::move(action), high_steady_clock::now() + delay});
}

// FIXME Consider running a task directly if it was added on the same thread
//       on which message_loop is spinning and it has no delay; use case: 
//       task is pushed on a loop and get() on its handle is called, in such
//       case application blocks
void message_loop::post_timed_task(timed_task t)
{
//  if (!active_) return;
//  LOG_DEBUG << "pushing a task on a thread: " << base::thread::current()->id();

  queue_.push(std::move(t));  
}

// TODO Add exception-safety
void message_loop::exec()
{
  while (active_)
  {
    auto tasks = queue_.wait_and_pop();
    for (auto& t : tasks)
      t.action();
  }
}
}
