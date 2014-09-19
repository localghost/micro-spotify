#ifndef BASE_ASSERT_H
#define BASE_ASSERT_H

#include <boost/assert.hpp>

#define PRECONDITION(cond) \
  BOOST_ASSERT_MSG(cond, "Precondition \"" #cond "\" violated")

#endif
