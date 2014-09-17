#ifndef ENGINE_TRACK_H
#define ENGINE_TRACK_H

#include <vector>
#include <string>
#include <libspotify/api.h>

//struct sp_track;
//struct sp_artist;

namespace engine {
class track FINAL
{
public:
  ~track();

  std::vector<sp_artist*> artists() const;
  sp_album* album() const;
  std::string name() const;
  unsigned duration() const;
  unsigned popularity() const;
  unsigned disc() const;
  unsigned index() const;
  void star(bool s);
  bool star() const;
  sp_track_availability availability() const;
  sp_track_offline_status offline_status() const;

private:
  explicit track(sp_track* track);

  sp_track* track;
};
}

#endif
