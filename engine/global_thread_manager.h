#ifndef ENGINE_GLOBAL_THREAD_MANAGER_H
#define ENGINE_GLOBAL_THREAD_MANAGER_H

#include <cstddef>
#include <base/thread.h>

namespace engine {
enum struct global_thread_id : std::size_t
{
  main_thread = 0, //ui_thread - TODO
  spotify_thread,
  thread_count
};

class global_thread_manager FINAL
{
public:
  static base::thread& get_thread(global_thread_id id);

private:
  static std::size_t thread_index(global_thread_id id);

  global_thread_manager();
  ~global_thread_manager();

  base::thread threads_[static_cast<std::size_t>(global_thread_id::thread_count)];
};

inline base::thread& spotify_thread()
{
  return global_thread_manager::get_thread(global_thread_id::spotify_thread);
}
}

#endif
