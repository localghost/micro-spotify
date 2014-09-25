#ifndef ENGINE_SESSION_H
#define ENGINE_SESSION_H

#include <cstdint>
#include <map>
#include <mutex>
#include <memory>
#include <unordered_set>
#include <boost/signals2.hpp>
#include <libspotify/api.h>
#include <base/thread.h>
#include <base/exception.h>
#include <base/request_map.h>
#include <engine/configuration.h>
#include <engine/search_request.h>
#include <engine/search_response.h>
#include <engine/player.h>
#include <engine/frame.h>

namespace engine {
typedef boost::error_info<struct spotify_error_info_t, sp_error> spotify_error_info;
EXCEPTION_TYPE(spotify_error);

class session FINAL
{
private:
  typedef boost::signals2::signal<void(sp_error)> logged_in_signal_type;
  typedef boost::signals2::signal<void()> logged_out_signal_type;
  typedef boost::signals2::signal<void(frame)> frames_delivered_signal_type;


public:
  typedef logged_in_signal_type::slot_type logged_in_slot_type;
  typedef logged_out_signal_type::slot_type logged_out_slot_type;
  typedef frames_delivered_signal_type::slot_type frames_delivered_slot_type;

  typedef std::function<void(search_response)> search_completed_callback;

  explicit session(configuration& config);
  ~session();

  void log_in();
  void log_out();

  //    playlist_container get_playlist_container();
  player get_player();
  void search(search_request request, search_completed_callback callback);

  boost::signals2::connection connect_logged_in(const logged_in_slot_type& slot);
  boost::signals2::connection connect_logged_out(const logged_out_slot_type& slot);
  boost::signals2::connection connect_frames_delivered(const frames_delivered_slot_type& slot);

private:
  struct search_request_data
  {
    session* self;
    search_completed_callback callback;
  };

  static sp_session_callbacks initialize_session_callbacks();

  static void log_message(sp_session* session_, const char* message);
  static void logged_in(sp_session* session_, sp_error error);
  static void logged_out(sp_session* session_);
  static int music_delivery(sp_session* session_,
                            const sp_audioformat* format,
                            const void* frames,
                            int num_frames);
  static void notify_main_thread(sp_session* session_);
  static void search_completed(sp_search* result, void* data);

  void create_session();
  void process_events();
  void notify_main_thread();

  static const sp_session_callbacks session_callbacks_;
  base::task_handle<void> process_events_handle_;
  base::task_handle<void> notify_main_thread_handle_;

  sp_session* session_;
  sp_session_config session_config_;

  logged_in_signal_type on_logged_in;
  logged_out_signal_type on_logged_out;
  frames_delivered_signal_type on_frames_delivered;

//  search_requests_type search_requests;
  //    std::map<sp_search*, search_request> search_requests;
  // FIXME Use either hash table or request_map
  std::vector<std::unique_ptr<search_request_data>> search_requests;
  std::mutex search_mutex;
};
}

#endif
