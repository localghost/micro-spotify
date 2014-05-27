#include <boost/test/unit_test.hpp>

#include <thread>
#include <chrono>
#include <base/task.h>
#include <base/message_loop.h>

namespace {
base::message_loop loop;

void publish()
{
  loop.queue_task(base::task{[]{std::cout << "task 1" << std::endl;}},
                  std::chrono::milliseconds{3000});
  loop.queue_task(base::task{[]{std::cout << "task 2" << std::endl;}});
  loop.queue_task(base::task{[]{loop.stop();}},
                  std::chrono::milliseconds{5000});
}
}

BOOST_AUTO_TEST_SUITE(TSBaseMessageLoop)

BOOST_AUTO_TEST_CASE(TCBaseMessageLoop)
{
  std::thread publisher{&publish};
  loop.start();
  publisher.join();
}

//BOOST_AUTO_TEST_CASE(TCEncodeLongString)
//{
//    BOOST_CHECK_EQUAL(base64::encode(data), expected);
//}
//
//BOOST_AUTO_TEST_CASE(TCEncodeUtf8String)
//{
//    const char* const data = "zażółć";
//    const char* const expected = "emHFvMOzxYLEhw==";
//    BOOST_CHECK_EQUAL(base64::encode(data), expected);
//}

BOOST_AUTO_TEST_SUITE_END()
