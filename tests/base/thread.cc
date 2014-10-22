#include <boost/test/unit_test.hpp>

#include <atomic>
#include <chrono>
#include <boost/scope_exit.hpp>
#include <base/task.h>
#include <base/thread.h>
#include <base/log.h>

BOOST_AUTO_TEST_SUITE(TSBaseThread)

BOOST_AUTO_TEST_CASE(TCBaseThread)
{
  std::atomic<int> counter{0};
  base::thread th;
  {
    th.start();
    BOOST_SCOPE_EXIT_ALL(&th) { th.stop(); };
    th.post_task(base::task<void>{[&counter]{ ++counter; }});
    th.post_task(base::task<void>{[&counter]{ ++counter; }});
  }
  BOOST_CHECK_EQUAL(counter, 2);
}

BOOST_AUTO_TEST_SUITE_END()
