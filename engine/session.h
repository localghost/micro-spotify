#ifndef ENGINE_SESSION_H
#define ENGINE_SESSION_H

#include <libspotify/api.h>

namespace engine {
class session
{
private:
    void setup_configuration();

    sp_session* session_;
};
}

#endif
