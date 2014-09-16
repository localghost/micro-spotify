#ifndef ENGINE_PLAYER_H
#define ENGINE_PLAYER_H

//#include <frame>
#include <libspotify/api.h>
#include <base/task.h>
#include <base/thread.h>

namespace engine {
//class player_impl;

class player
{
  typedef boost::signals2::signal<unsigned(const frame&)> sink_sinal_type;

public:
  typedef sink_signal_type::slot_type sink_slot_type;

  void play(sp_track* track)
  {
    spotify_thread().queue_task(base::make_task([this, track]
          {
            sp_session_player_load(session_, track);
            sp_session_player_play(session_, true);
          });
  }
//  void pause();
//  void seek();

  boost::signals2::connection connect_sink(const sink_slot_type& sink);

private:
  friend class session;

  explicit player(sp_session* session_) : session_{session_} {}

  sp_session* session_;

//  explicit player(player_impl* impl);
//
//  std::shared_ptr<player_impl> impl;
};
}

#endif
