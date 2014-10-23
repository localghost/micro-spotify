#ifndef ENGINE_GLOBAL_THREAD_MANAGER_H
#define ENGINE_GLOBAL_THREAD_MANAGER_H

#include <cstddef>
#include <base/thread.h>
#include <base/export.h>

namespace engine {
enum struct EXPORT_API global_thread_id : std::size_t
{
  main_thread = 0, //ui_thread - TODO
  spotify_thread,
  audio_thread,
  thread_count
};

class EXPORT_API global_thread_manager FINAL
{
public:
  static base::thread& get_thread(global_thread_id id);

private:
  static std::size_t thread_index(global_thread_id id);

  global_thread_manager();
  ~global_thread_manager();

  base::thread threads_[static_cast<std::size_t>(global_thread_id::thread_count)];
};

#define DEFINE_THREAD_ACCESSOR(thread_id) \
  inline base::thread& thread_id() \
  { \
    return global_thread_manager::get_thread(global_thread_id::thread_id); \
  }

DEFINE_THREAD_ACCESSOR(spotify_thread)
DEFINE_THREAD_ACCESSOR(audio_thread)

}

#endif
