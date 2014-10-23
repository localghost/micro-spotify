#ifndef ENGINGE_SEARCH_RESPONSE_H
#define ENGINGE_SEARCH_RESPONSE_H

#include <memory>
#include <libspotify/api.h>
#include <base/export.h>

namespace engine {
class search_response_impl;

class EXPORT_API search_response
{
public:
  std::string query();
  std::string did_you_mean();

  size_t num_tracks();
  sp_track* track(size_t num);
  size_t total_tracks();

  size_t num_albums();
  sp_album* album(size_t num);
  size_t total_albums();

  size_t num_playlists();
  sp_playlist* playlist(size_t num);
  size_t total_playlists();

  size_t num_artists();
  sp_artist* artist(size_t num);
  size_t total_artists();

private:
  friend class session;

  explicit search_response(search_response_impl*);

  std::shared_ptr<search_response_impl> impl;
};
}

#endif
