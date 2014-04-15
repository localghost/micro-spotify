#include "session.h"

namespace engine {
namespace {
const char* const application_key = "";
}

session::session(configuration& config)
{
  // setup configuration
  std::string cache_location = config.cache.value();

  sp_session_config config;
  config.api_version = SPOTIFY_API_VERSION;
  config.userdata = this;
  // FIXME duplicate configuration string since they may change
  //       by external source
  config.cache_location = cache_location.c_str();
  config.application_key = application_key;
  ...;
  // setup callbacks
  sp_error error = sp_session_create(&config, &session_);
  if (SP_ERROR_OK != error)
    THROW(EXCEPTION(spotify_error) << error_info::sp_error{error});
}

session::~session()
{
  sp_session_release(session_);
}
}

#endif
