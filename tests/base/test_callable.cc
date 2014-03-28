#include <boost/test/unit_test.hpp>

#include <base/callable.h>

BOOST_AUTO_TEST_SUITE(TSBaseCallable)

BOOST_AUTO_TEST_CASE(TCBaseCallableNoArgs)
{
    bool pass = false;
    base::callable call{[&pass]() { pass = true; }};
    call();
    BOOST_CHECK(pass);
}

BOOST_AUTO_TEST_SUITE_END()
