#include "global_threads_manager.h"

#include <cassert>

namespace engine {
base::thread& global_threads_manager::get_thread(global_thread_id id)
{
  std::assert(thread_index(id) < thread_index(global_thread_id::thread_count));

  static global_threads_manager gtm;

  return gtm.threads_[thread_index(id)];
}

inline std::size_t global_threads_manager::thread_index(global_thread_id id)
{
  return static_cast<std::size_t>(id);
}

global_threads_manager::global_threads_manager()
{
  // FIXME What if some code in main_thread queues a task to a spotify_thread
  //       before it is started? Currently, an assert will be thrown so either
  //       this piece of code or base::thread::start() requires redesign
  //       see https://github.com/localghost/micro-spotify/wiki/Architecture:-Threading#queue-a-task-to-a-not-running-thread
  for (std::size_t i = 0; i < thread_index(global_thread_id::thread_count); ++i)
    threads_[i].start();
}

global_threads_manager::~global_threads_manager()
{
  for (std::size_t i = thread_index(global_thread_id::thread_count) - 1; i >= 0; --i)
    threads_[i].stop();
}
}
