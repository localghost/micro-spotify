#ifndef ENGINE_EXCEPTION_H
#define ENGINE_EXCEPTION_H

#include <libspotify.h>
#include <base/exception.h>

namespace engine {
namespace error_info {
#if SUPPORTED(CPP11)
using sp_error_code = boost::error_info<struct error_info_sp_error_, ::sp_error>;
#else
typedef boost::error_info<struct error_info_sp_error_, ::sp_error> sp_error;
#endif
}
EXCEPTION_TYPE(spotify_error);
}

#endif
