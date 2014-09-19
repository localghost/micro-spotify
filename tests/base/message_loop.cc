#include <boost/test/unit_test.hpp>

#include <thread>
#include <chrono>
#include <functional>
#include <vector>
#include <base/task.h>
#include <base/message_loop.h>
#include <base/assert.h>
#include <base/chrono.h>

namespace {
void publish(base::message_loop& loop, std::vector<int>& values)
{
  loop.queue_task(base::task<void>{[&]{ values.push_back(1); }}, 1000_ms);
  loop.queue_task(base::task<void>{[&]{ values.push_back(2); }});
  loop.queue_task(base::task<void>{[&loop]{loop.stop();}}, 1100_ms);
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
  std::vector<int> values;
  std::thread subscriber{&subscribe, std::ref(loop)};
  std::thread publisher{&publish, std::ref(loop), std::ref(values)};
  publisher.join();
  subscriber.join();

  BOOST_CHECK_EQUAL(values.size(), 2);
  BOOST_CHECK_EQUAL(values.at(0), 2);
  BOOST_CHECK_EQUAL(values.at(1), 1);
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
