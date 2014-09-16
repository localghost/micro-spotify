#ifndef ENGINE_FRAME_H
#define ENGINE_FRAME_H

namespace engine {
// FIXME logically a frame should be a copyable object
//       this is a quick fix
struct frame : boost::noncopyable
{
  unsigned rate;
  unsigned channels;
  unsigned samples;
  const void* data;
};
}
#endif
