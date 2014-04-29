#ifndef BASE_TRAITS_H
#define BASE_TRAITS_H

#include <chrono>

namespace base {
namespace detail {
template <bool> struct high_steady_clock;

template <> struct high_steady_clock<true>
{
  typedef std::chrono::high_resolution_clock type;
};

template <> struct high_steady_clock<false>
{
  typedef std::chrono::steady_clock type;
};
}

typedef detail::high_steady_clock<std::chrono::high_resolution_clock::is_steady>::type high_steady_clock;
}

#endif
