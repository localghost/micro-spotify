#include "search_response_impl.h"

#include <libspotify/api.h>
#include <base/exception.h>
#include <base/task.h>
#include <base/thread.h>
#include "global_thread_manager.h"

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
  auto t = base::make_task([this]() -> std::string
      {
        const char* query = sp_search_query(search);
        return (query ? query : "");
      });
  return base::post_task_with_handle(spotify_thread(), std::move(t)).get();
}

std::string search_response_impl::did_you_mean() 
{
  // TODO implement me
  return "";
}

size_t search_response_impl::num_tracks()
{
  return static_cast<size_t>(sp_search_num_tracks(search));
}

sp_track* search_response_impl::track(size_t num)
{
  auto t = base::make_task([this, num]
      {
        sp_track* result = sp_search_track(search, static_cast<int>(num));
        // FIXME Can it happen only when num > sp_search_nm_tracks - 1?
        if (!result)
          THROW(base::out_of_bounds_error{});
        return result;
      });
  return base::post_task_with_handle(spotify_thread(), std::move(t)).get();
}

size_t search_response_impl::total_tracks()
{
  // TODO implement me
  return 0;
}

size_t search_response_impl::num_albums()
{
  // TODO implement me
  return 0;
}

sp_album* search_response_impl::album(size_t /*num*/) {}

size_t search_response_impl::total_albums()
{
  // TODO implement me
  return 0;
}

size_t search_response_impl::num_playlists()
{
  // TODO implement me
  return 0;
}

sp_playlist* search_response_impl::playlist(size_t /*num*/) {}

size_t search_response_impl::total_playlists()
{
  // TODO implement me
  return 0;
}

size_t search_response_impl::num_artists()
{
  // TODO implement me
  return 0;
}

sp_artist* search_response_impl::artist(size_t /*num*/) {}

size_t search_response_impl::total_artists()
{
  // TODO implement me
  return 0;
}
}
