#ifndef ENGINE_SEARCH_H
#define ENGINE_SEARCH_H

#include <string>
#include <boost/signals2.hpp>

namespace engine {
struct search_query
{
  typedef boost::signals2::signal<void(search_result)> search_completed_signal_type;

  std::string query;
  
  size_t track_offset;
  size_t track_count;

  size_t album_offset;
  size_t album_count;

  size_t artist_offset;
  size_t artist_count;

  size_t playlist_offset;
  size_t playlist_count;

  search_completed_signal_type search_completed;
};
}

#endif
