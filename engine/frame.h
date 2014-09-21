#ifndef ENGINE_FRAME_H
#define ENGINE_FRAME_H

#include <memory>

namespace engine {
// FIXME logically a frame should be a copyable object
//       this is a quick fix
struct frame
{
  unsigned rate;
  unsigned channels;
  unsigned samples;
  std::shared_ptr<const int16_t> data;
};
}
#endif
