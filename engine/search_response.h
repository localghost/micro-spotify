#ifndef ENGINGE_SEARCH_RESPONSE_H
#define ENGINGE_SEARCH_RESPONSE_H

#include <memory>

namespace engine {
class search_response_impl;

class search_response
{
public:
  std::string query();
  std::string did_you_mean();

  size_t num_tracks();
  /*track*/ void track(size_t num);
  size_t total_tracks();

  size_t num_albums();
  /*album*/ void album(size_t num);
  size_t total_albums();

  size_t num_playlists();
  /*playlist*/ void playlist(size_t num);
  /* playlist related uri? */
  size_t total_playlists();

  size_t num_artists();
  /*artist*/ void artist(size_t num);
  size_t total_artists();

private:
  friend class session;

  explicit search_response(search_response_impl*);

  std::unique_ptr<search_response_impl> impl;
};
}

#endif
