#include <boost/test/unit_test.hpp>

#include <atomic>
#include <base/scoped_thread.h>
#include <base/log.h>
#include <base/chrono.h>

BOOST_AUTO_TEST_SUITE(TSBaseScopedThread)

BOOST_AUTO_TEST_CASE(TCBaseScopedThread)
{
  std::atomic<bool> joined{false};
  {
    base::scoped_thread([&joined]
      {
        std::this_thread::sleep_for(1000_ms);
        joined = true;
      }
    );
  }
  BOOST_CHECK(joined);
}

BOOST_AUTO_TEST_SUITE_END()
