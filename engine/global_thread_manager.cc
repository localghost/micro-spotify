#include "global_thread_manager.h"

#include <base/log.h>
#include <base/assert.h>

namespace engine {
base::thread& global_thread_manager::get_thread(global_thread_id id)
{
  PRECONDITION(thread_index(id) < thread_index(global_thread_id::thread_count));

  static global_thread_manager gtm;

  return gtm.threads_[thread_index(id)];
}

inline std::size_t global_thread_manager::thread_index(global_thread_id id)
{
  return static_cast<std::size_t>(id);
}

global_thread_manager::global_thread_manager()
{
  // FIXME There is a race with something!!! (probably some other static)
  LOG_DEBUG << "global_thread_manager starting up...";
  // FIXME What if some code in main_thread queues a task to a spotify_thread
  //       before it is started? Currently, an assert will be thrown so either
  //       this piece of code or base::thread::start() requires redesign
  //       see https://github.com/localghost/micro-spotify/wiki/Architecture:-Threading#queue-a-task-to-a-not-running-thread
  for (std::size_t i = 0; i < thread_index(global_thread_id::thread_count); ++i)
    threads_[i].start();
}

global_thread_manager::~global_thread_manager()
{
  std::size_t i = thread_index(global_thread_id::thread_count);
  while (i-- != 0)
    threads_[i].stop();
}
}
