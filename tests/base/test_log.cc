#include <boost/test/unit_test.hpp>

#include <base/log.h>

BOOST_AUTO_TEST_SUITE(TSBaseLog)

BOOST_AUTO_TEST_CASE(TCBaseLogDebug)
{
    LOG_DEBUG << "Test debug log";
}

BOOST_AUTO_TEST_CASE(TCBaseLogInfo)
{
    LOG_INFO << "Test info log " << 42;
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
