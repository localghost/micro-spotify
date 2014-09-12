#ifndef ENGINE_PLAYLIST_CONTAINER_H
#define ENGINE_PLAYLIST_CONTAINER_H

namespace engine {
class playlist_container
{
public:
  playlist create();
  playlist get(size_t n);
  void remove(size_t n);
  void move(size_t n, size_t position);

  size_t size(); // sp_playlistcontainer_num_playlists()


private:
  playlist_container(sp_playlist_container* container) : container{container} {}

  sp_playlistcontainer* container;
};
}

#endif
