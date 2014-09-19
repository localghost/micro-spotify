#ifndef BASE_CHRONO_H
#define BASE_CHRONO_H

#include <chrono>
#include <type_traits>

namespace base {
//namespace detail {
//template <bool> struct high_steady_clock;
//
//template <> struct high_steady_clock<true>
//{
//  typedef std::chrono::high_resolution_clock type;
//};
//
//template <> struct high_steady_clock<false>
//{
//  typedef std::chrono::steady_clock type;
//};
//}

typedef std::conditional<std::chrono::high_resolution_clock::is_steady,
                         std::chrono::high_resolution_clock,
                         std::chrono::steady_clock>::type
                         high_steady_clock;

typedef high_steady_clock::duration time_delay;
}

inline std::chrono::milliseconds operator"" _ms(unsigned long long milliseconds)
{
  return std::chrono::milliseconds{milliseconds};
}

#endif
