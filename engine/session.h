#ifndef ENGINE_SESSION_H
#define ENGINE_SESSION_H

#include <libspotify/api.h>

namespace engine {
class session FINAL
{
    typedef boost::signals2::signal<void(sp_error)> logged_in_signal_type;

public:
    typedef logged_in_signal_type::slot_type logged_in_slot_type;

    explicit session(configuration& config);
    ~session();

    boost::signals2::scoped_connection connect_logged_in(const logged_in_slot_type& slot);

private:
    static void logged_in(sp_session* session, sp_error error);
    static int music_delivery(sp_session* session,
                              const sp_audioformat* format,
                              const void* frames,
                              int num_frames);

    sp_session* session_;
};
}

#endif
