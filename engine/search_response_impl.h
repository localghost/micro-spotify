#ifndef ENGINE_SEARCH_RESPONSE_IMPL_H
#define ENGINE_SEARCH_RESPONSE_IMPL_H

#include <string>
#include <libspotify/api.h>

struct sp_search;

namespace engine {
class search_response_impl
{
public:
  explicit search_response_impl(sp_search* search);
  ~search_response_impl();

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
  sp_search* search = nullptr;
};
}

#endif
