#include <boost/test/unit_test.hpp>

#include <thread>
#include <chrono>
#include <functional>
#include <base/task.h>
#include <base/message_loop.h>

namespace {
void publish(base::message_loop& loop)
{
  loop.queue_task(base::task<void>{[]{std::cout << "task 1" << std::endl;}},
                  std::chrono::milliseconds{3000});
  loop.queue_task(base::task<void>{[]{std::cout << "task 2" << std::endl;}});
  loop.queue_task(base::task<void>{[&loop]{loop.stop();}},
                  std::chrono::milliseconds{5000});
}

void subscribe(base::message_loop& loop)
{
  loop.start();
}
}

BOOST_AUTO_TEST_SUITE(TSBaseMessageLoop)

BOOST_AUTO_TEST_CASE(TCBaseMessageLoop)
{
  base::message_loop loop;
  std::thread subscriber{&subscribe, std::ref(loop)};
  std::thread publisher{&publish, std::ref(loop)};
  publisher.join();
  subscriber.join();
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
