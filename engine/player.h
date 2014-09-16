#ifndef ENGINE_PLAYER_H
#define ENGINE_PLAYER_H

#include <frame>

namespace engine {
class player_impl;

class player
{
  typedef boost::signals2::signal<unsigned(const frame&)> sink_sinal_type;

public:
  typedef sink_signal_type::slot_type sink_slot_type;

  void play(/*track*/);
  void pause();
  void seek();

  boost::signals2::connection connect_sink(const sink_slot_type& sink);

private:
  friend class session;

  explicit player(player_impl* impl);

  std::shared_ptr<player_impl> impl;
};
}

#endif
