#include "search_response.h"
#include "search_response_impl.h"

#include <base/assert.h>

namespace engine {
std::string search_response::query()
{
  return impl->query();
}

std::string search_response::did_you_mean()
{
  return impl->did_you_mean();
}

size_t search_response::num_tracks()
{
  return impl->num_tracks();
}

/*track*/ void search_response::track(size_t num)
{
  return impl->track(num);
}

size_t search_response::total_tracks()
{
  return impl->total_tracks();
}

size_t search_response::num_albums()
{
  return impl->num_albums();
}

/*album*/ void search_response::album(size_t num)
{
  return impl->album(num);
}

size_t search_response::total_albums()
{
  return impl->total_albums();
}

size_t search_response::num_playlists()
{
  return impl->num_playlists();
}

/*playlist*/ void search_response::playlist(size_t num)
{
  return impl->playlist(num);
}

size_t search_response::total_playlists()
{
  return impl->total_playlists();
}

size_t search_response::num_artists()
{
  return impl->num_artists();
}

/*artist*/ void search_response::artist(size_t num)
{
  return impl->artist(num);
}

size_t search_response::total_artists()
{
  return impl->total_artists();
}

search_response::search_response(search_response_impl* impl) : impl{impl}
{
  BOOST_ASSERT_MSG(impl, "Empty implementation");
}
}
