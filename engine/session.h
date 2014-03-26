#ifndef ENGINE_SESSION_H
#define ENGINE_SESSION_H

#include <libspotify/api.h>

namespace engine {
class session FINAL
{
    typedef boost::signals2::signal<void(sp_error)> logged_in_signal_type;

public:
    typedef logged_in_signal_type::slot_type logged_in_slot_type;

    session();
    ~session();



    boost::signals2::scoped_connection connect_logged_in(const logged_in_slot_type& slot);

private:
    void setup_configuration();

    sp_session* session_;
};
}

#endif
