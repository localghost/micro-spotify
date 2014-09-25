#ifndef ENGINE_SEARCH_REQUEST_H
#define ENGINE_SEARCH_REQUEST_H

#include <string>

namespace engine {
struct search_request
{
  std::string query;
  
  size_t track_offset;
  size_t track_count;

  size_t album_offset;
  size_t album_count;

  size_t artist_offset;
  size_t artist_count;

  size_t playlist_offset;
  size_t playlist_count;
};
}

#endif
