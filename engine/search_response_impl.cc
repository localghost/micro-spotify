#include "search_response_impl.h"

#include <libspotify/api.h>
#include <base/exception.h>

namespace engine {
search_response_impl::search_response_impl(sp_search* search)
  : search{search}
{}

search_response_impl::~search_response_impl()
{
  sp_search_release(search);
}

std::string search_response_impl::query()
{
  const char* query = sp_search_query(search);
  return (query ? query : "");
}

size_t search_response_impl::num_tracks()
{
  return size_t{sp_search_num_tracks(search)};
}

void search_response_impl::track(size_t num)
{
  sp_track* result = sp_search_track(search, int{num});
  // FIXME Can it happen only when num > sp_search_nm_tracks - 1?
  if (!result)
    THROW(base::out_of_bounds_error{});
  // return track{result};
}

size_t search_response_impl::total_tracks() {}

size_t search_response_impl::num_albums() {}
void search_response_impl::album(size_t num) {}
size_t search_response_impl::total_albums() {}

size_t search_response_impl::num_playlists() {}
void search_response_impl::playlist(size_t num) {}
size_t search_response_impl::total_playlists() {}

size_t search_response_impl::num_artists() {}
void search_response_impl::artist(size_t num) {}
size_t search_response_impl::total_artists() {}

std::string search_response_impl::did_you_mean() {}
}
