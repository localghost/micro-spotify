#ifndef ENGINE_SEARCH_REQUEST_H
#define ENGINE_SEARCH_REQUEST_H

#include <string>
#include <boost/signals2.hpp>

#include <engine/search_response.h>

namespace engine {
struct search_request
{
  typedef boost::signals2::signal<void(search_response)> search_completed_signal_type;

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

  search_request() = default;

  search_request(search_request&& other) noexcept
  {
    move(std::move(other));
  }

  search_request& operator=(search_request&& other) noexcept
  {
    move(std::move(other));
    return *this;
  }

private:
  void move(search_request&& other)
  {
    query = std::move(other.query);
    track_offset = std::move(other.track_offset);
    track_count = std::move(other.track_count);
    album_offset = std::move(other.album_offset);
    album_count = std::move(other.album_count);
    artist_offset = std::move(other.artist_offset);
    artist_count = std::move(other.artist_count);
    playlist_offset = std::move(other.playlist_offset);
    playlist_count = std::move(other.playlist_count);

    // FIXME This is not a perfect solution, temporary quick-fix ;)
    //       disconnect_all_slots() may throw but users of move
    //       are marked as noexcept (result -> std::terminate())
    search_completed.disconnect_all_slots();
    search_completed.swap(other.search_completed);
  }
};
}

#endif
