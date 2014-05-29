#ifndef ENGINE_GLOBAL_THREADS_MANAGER_H
#define ENGINE_GLOBAL_THREADS_MANAGER_H

#include <cstddef>
#include <base/thread.h>

namespace engine {
enum struct global_thread_id : std::size_t
{
  main_thread = 0, //ui_thread - TODO
  spotify_thread,
  thread_count
};

class global_threads_manager FINAL
{
public:
  static base::thread& get_thread();

private:
  static std::size_t thread_index(global_thread_id id) const;

  global_threads_manager();
  ~global_threads_manager();

  base::thread threads_[global_thread_id::thread_count];
};

inline base::thread& spotify_thread()
{
  return global_threads_manager::get_thread(global_thread_id::spotify_thread);
}
}

#endif
