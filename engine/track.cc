#include "track.h"

namespace engine {
std::vector<sp_artists*> track::artists() const
{
  // TODO implement me
  return {};
}

sp_album* track::album() const
{
  // TODO implement me
  return nullptr;
}

std::string track::name() const
{
  // TODO implement me
  return "";
}

unsigned track::duration() const
{
  // TODO implement me
  return 0;
}

unsigned track::popularity() const
{
  // TODO implement me
  return 0;
}

unsigned track::disc() const
{
  // TODO implement me
  return 0;
}

unsigned track::index() const
{
  // TODO implement me
  return 0;
}

void track::star(bool /*s*/)
{
  // TODO implement me
}

bool track::star() const
{
  // TODO implement me
  return false;
}

sp_track_availability track::availability() const
{
  // TODO implement me
  return {};
}

sp_track_offline_status track::offline_status() const
{
  // TODO implement me
  return {};
}

track::track(sp_track* track) : track{track}
{
  sp_track_add_ref(track);
}

track::~track()
{
  sp_track_release(track);
}
}
