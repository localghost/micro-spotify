#include "session.h"

#include <cstdint>
#include <cstring>

#include <base/task.h>
#include <base/log.h>
#include <base/assert.h>

#include "global_thread_manager.h"

namespace signals = boost::signals2;

namespace engine {
namespace {
const std::uint8_t application_key[] = {
    0x01, 0x59, 0x32, 0xDF, 0xA5, 0xD1, 0xFE, 0x73, 0xA1, 0x68, 0x85, 0xE3, 0x9B, 0x84, 0xF5, 0x51,
    0x5F, 0xBF, 0xF3, 0xE8, 0x19, 0xE4, 0x96, 0x57, 0xF1, 0x70, 0x6F, 0xB5, 0x1F, 0x16, 0xB2, 0xAF,
    0x34, 0x58, 0x1D, 0x7C, 0x9E, 0xEC, 0x8E, 0x5F, 0x3E, 0x69, 0x25, 0xBD, 0xC7, 0xAE, 0x01, 0xBE,
    0x5D, 0x25, 0x9A, 0x4D, 0xD0, 0x3D, 0x57, 0x12, 0x3F, 0x43, 0x38, 0x4E, 0xA7, 0x93, 0xAD, 0x51,
    0x64, 0x14, 0x9C, 0xCC, 0xFB, 0xA5, 0x33, 0xF4, 0x01, 0xEA, 0x78, 0x8A, 0x63, 0x56, 0x0B, 0xA6,
    0x62, 0x14, 0xD4, 0x8D, 0x31, 0x86, 0x7A, 0xB2, 0x73, 0xBE, 0x96, 0xC9, 0xDE, 0x41, 0x79, 0x48,
    0x60, 0x89, 0xA5, 0xCE, 0x4B, 0x62, 0x0C, 0xE5, 0x4E, 0x85, 0xDB, 0xA8, 0x36, 0x89, 0x88, 0x78,
    0x6F, 0x9B, 0xE7, 0x44, 0x28, 0xAC, 0xAF, 0x6F, 0xA1, 0x91, 0x83, 0xCA, 0x94, 0x0C, 0x5E, 0xED,
    0xE8, 0x0E, 0x83, 0x73, 0xCF, 0x3F, 0x08, 0x35, 0x63, 0xD6, 0x11, 0x09, 0x18, 0xB8, 0x20, 0x1E,
    0xD9, 0xC6, 0x50, 0x69, 0x72, 0x24, 0x68, 0xED, 0x23, 0x25, 0xC6, 0x99, 0x73, 0x1F, 0x25, 0x02,
    0x30, 0x4C, 0x73, 0x5A, 0x98, 0x9F, 0x9A, 0x3E, 0x8C, 0x98, 0xB6, 0x2D, 0xC6, 0x9E, 0x8A, 0x9C,
    0xC5, 0x3D, 0x2B, 0xAC, 0x4E, 0x72, 0xA7, 0x7D, 0x44, 0xE2, 0x11, 0x8E, 0x3C, 0xCE, 0xCE, 0x5E,
    0xE5, 0x73, 0x53, 0xC7, 0xA1, 0x78, 0x72, 0xF5, 0x27, 0x61, 0x8A, 0x6E, 0xF0, 0x2E, 0x29, 0x14,
    0xC3, 0xA9, 0x1C, 0x1B, 0x34, 0x75, 0x97, 0xC8, 0xC4, 0x69, 0x4C, 0x41, 0xC1, 0x18, 0x2D, 0x54,
    0xB2, 0x12, 0x71, 0x96, 0x93, 0x45, 0x16, 0x9E, 0x64, 0xDD, 0xB9, 0xD3, 0x87, 0x49, 0x1A, 0x79,
    0x49, 0x88, 0xF2, 0x2D, 0xBF, 0x41, 0xF9, 0xEF, 0x55, 0x77, 0xCF, 0xFF, 0x89, 0xA8, 0xF6, 0xBB,
    0x98, 0xF3, 0xA4, 0xD0, 0x5B, 0x61, 0x8B, 0xD1, 0x14, 0xC6, 0xFA, 0x3F, 0x41, 0x20, 0xA7, 0x79,
    0xC8, 0x04, 0x5B, 0xFE, 0xF7, 0xCE, 0x08, 0x73, 0xB3, 0x0C, 0x3C, 0x2E, 0xF2, 0x93, 0x9B, 0x77,
    0x6C, 0xB4, 0x0D, 0xAF, 0xA3, 0x0C, 0xFA, 0x47, 0xD1, 0xAB, 0x2A, 0x08, 0x94, 0x56, 0x92, 0x5D,
    0x8E, 0x2A, 0x83, 0xB4, 0x9B, 0x16, 0xE6, 0x2F, 0x04, 0xE6, 0xF5, 0x5A, 0x2F, 0x9E, 0xFD, 0x97,
    0x49,
};
const char username[] = "";
const char password[] = "";
const char application_name[] = "micro-spotify";
}

const sp_session_callbacks session::session_callbacks_ = session::initialize_session_callbacks();

session::session(configuration& /*config*/)
{
  BOOST_ASSERT_MSG(base::thread::current()->id() != spotify_thread().id(),
                   "Session object MUST NOT be created on spotify thread");
  // setup configuration
//  std::string cache_location = config.cache.value();

  std::memset(&session_config_, 0, sizeof(session_config_));
  session_config_.api_version = SPOTIFY_API_VERSION;
  session_config_.userdata = this;
  // FIXME duplicate configuration string since they may change
  //       by external source
  session_config_.cache_location = "/tmp";//cache_location.c_str();
  session_config_.settings_location = "/tmp";
  session_config_.application_key = application_key;
  session_config_.application_key_size = sizeof(application_key);
  session_config_.user_agent = application_name;
  session_config_.callbacks = &session_callbacks_;

  base::queue_task_with_handle(spotify_thread(),
                               base::make_task(&session::create_session, this)).get();
  spotify_thread().queue_task(base::make_task(&session::process_events, this));
}

session::~session()
{
  // FIXME clean up and shutdown everything nicely

  // assuming for now that there is only one session object and when it is destroyed
  // then it is as if the application was being shutdown and it is ok to clean
  // the queue of the spotify thread

  if (process_events_handle_.is_valid() && !process_events_handle_.cancel())
    base::queue_task_with_handle(spotify_thread(),
                                 base::make_task([this]{ process_events_handle_.cancel(); })).get();

  base::queue_task_with_handle(spotify_thread(),
                               base::make_task(&sp_session_release, session_)).get();
}

void session::log_in()
{
  spotify_thread().queue_task(base::make_task(&sp_session_login,
                                              session_,
                                              username,
                                              password,
                                              false,
                                              nullptr));
}

void session::log_out()
{
  spotify_thread().queue_task(base::make_task(&sp_session_logout, session_));
}

signals::connection session::connect_logged_in(const logged_in_slot_type& slot)
{
  return on_logged_in.connect(slot);
}

signals::connection session::connect_logged_out(const logged_out_slot_type& slot)
{
  return on_logged_out.connect(slot);
}

signals::connection session::connect_frames_delivered(const frames_delivered_slot_type& slot)
{
  return on_frames_delivered.connect(slot);
}

void session::log_message(sp_session* session_, const char* message)
{
  LOG_DEBUG << "Message from spotify for [" << session_ << "] " << message;
}

void session::logged_in(sp_session* session_, sp_error error)
{
  session* self = static_cast<session*>(sp_session_userdata(session_));
  assert(self);
  self->on_logged_in(error);
}

void session::logged_out(sp_session* session_)
{
  session* self = static_cast<session*>(sp_session_userdata(session_));
  assert(self);
  self->on_logged_out();
}

void session::notify_main_thread(sp_session* session_)
{
  assert(session_);
  session* self = static_cast<session*>(sp_session_userdata(session_));
  spotify_thread().queue_task(
      base::make_task(static_cast<void(session::*)()>(&session::notify_main_thread), self));
}

int session::music_delivery(sp_session* /*session_*/,
                            const sp_audioformat* /*format*/,
                            const void* /*frames*/,
                            int /*num_frames*/)
{
  return 0;
}

void session::process_events()
{
  BOOST_ASSERT(base::thread::current()->id() == spotify_thread().id());
  int timeout = 0;
  sp_session_process_events(session_, &timeout);
  process_events_handle_
      = base::queue_task_with_handle(spotify_thread(),
                                     base::make_task(&session::process_events, this),
                                     std::chrono::milliseconds{timeout});
}

void session::create_session()
{
  BOOST_ASSERT(base::thread::current()->id() == spotify_thread().id());
  sp_error error = sp_session_create(&session_config_, &session_);
  if (SP_ERROR_OK != error)
    THROW(spotify_error{} << spotify_error_info{error});
}

void session::notify_main_thread()
{
  BOOST_ASSERT(base::thread::current()->id() == spotify_thread().id());
  try
  {
    // this should always succeed (except when processing events has not been yet)
    // as task is executed on the same thread as this code
    process_events_handle_.cancel();
    process_events();
  }
  catch (base::task_error)
  {
    LOG_ERROR << "notify_main_thread() called before events processing started!\n"
      << boost::current_exception_diagnostic_information();
    // assuming processing events will be started sooner or later
    int timeout = 0;
    sp_session_process_events(session_, &timeout);
  }
}

sp_session_callbacks session::initialize_session_callbacks()
{
  sp_session_callbacks result;
  result.logged_in = &logged_in;
  result.logged_out = &logged_out;
  result.log_message = &log_message;
  result.notify_main_thread = &notify_main_thread;
  result.music_delivery = &music_delivery;
  return result;
}
}
