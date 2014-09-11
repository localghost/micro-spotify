#ifndef ENGINE_SESSION_H
#define ENGINE_SESSION_H

#include <boost/signals2.hpp>
#include <libspotify/api.h>
#include <base/thread.h>
#include <base/exception.h>
#include <engine/configuration.h>

namespace engine {
typedef boost::error_info<struct spotify_error_info_t, sp_error> spotify_error_info;
EXCEPTION_TYPE(spotify_error);

class session FINAL
{
    typedef boost::signals2::signal<void(sp_error)> logged_in_signal_type;
    typedef boost::signals2::signal<void(sp_error)> frames_delivered_signal_type;

public:
    typedef logged_in_signal_type::slot_type logged_in_slot_type;
    typedef frames_delivered_signal_type::slot_type frames_delivered_slot_type;

    explicit session(configuration& config);
    ~session();

    void log_in();

    boost::signals2::connection connect_logged_in(const logged_in_slot_type& slot);
    boost::signals2::connection connect_frames_delivered(const frames_delivered_slot_type& slot);

private:
    static sp_session_callbacks initialize_session_callbacks();

    static void logged_in(sp_session* session, sp_error error);
    static int music_delivery(sp_session* session_,
                              const sp_audioformat* format,
                              const void* frames,
                              int num_frames);
    static void notify_main_thread(sp_session* session_);

    void create_session();
    void process_events();
    void notify_main_thread();

    static const sp_session_callbacks session_callbacks_;
    base::task_handle<void> process_events_handle_;

    sp_session* session_;
    sp_session_config session_config_;

    logged_in_signal_type on_logged_in;
    frames_delivered_signal_type on_frames_delivered;
};
}

#endif
