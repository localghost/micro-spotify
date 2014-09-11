#ifndef BASE_ASSERT_H
#define BASE_ASSERT_H

#include <boost/assert.hpp>

#undef ASSERT_EQ
#define ASSERT_EQ(value, expected) \
  BOOST_ASSERT((value) == (expected))

#endif
