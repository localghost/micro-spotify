#include <boost/test/unit_test.hpp>

#include <functional>
#include <base/callable.h>

BOOST_AUTO_TEST_SUITE(TSBaseCallable)

/// Uses callable with lambda
BOOST_AUTO_TEST_CASE(TCCallable1)
{
  unsigned counter = 0;

  base::callable{[&]{++counter;}}();

  BOOST_CHECK_EQUAL(counter, 1);
}

/// Uses callable with a functor
BOOST_AUTO_TEST_CASE(TCCallable2)
{
  struct callee
  {
    explicit callee(unsigned& counter) : counter{counter} {}
    void operator()()
    {
      ++counter;
    }
    unsigned& counter;
  };
  unsigned counter = 0;

  base::callable{callee{counter}}();

  BOOST_CHECK_EQUAL(counter, 1);
}

/// Uses callable with a method (via std::bind)
BOOST_AUTO_TEST_CASE(TCCallable3)
{
  struct callee
  {
    explicit callee(unsigned& counter) : counter{counter} {}
    void call()
    {
      ++counter;
    }
    unsigned& counter;
  };

  unsigned counter = 0;
  callee action{counter};

  base::callable{std::bind(&callee::call, &action)}();

  BOOST_CHECK_EQUAL(counter, 1);
}

namespace {
void TCCallable4_callee(unsigned& counter)
{
  ++counter;
}
}
/// Uses callable with a function (via std::bind)
BOOST_AUTO_TEST_CASE(TCCallable4)
{
  unsigned counter = 0;

  base::callable{std::bind(&TCCallable4_callee, std::ref(counter))}();

  BOOST_CHECK_EQUAL(counter, 1);
}

BOOST_AUTO_TEST_SUITE_END()
