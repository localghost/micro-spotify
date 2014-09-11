#include <boost/test/unit_test.hpp>

#include <base/exception.h>
#include <engine/configuration.h>
#include <engine/session.h>

BOOST_AUTO_TEST_SUITE(TSEngineSession)

BOOST_AUTO_TEST_CASE(TCEngineSession)
{
  bool success = false;
  std::string message;

  try 
  {
    engine::configuration config;
    engine::session s{config};
  }
  catch (base::exception)
  {
    success = false;
    message = boost::current_exception_diagnostic_information();
  }

  BOOST_CHECK_MESSAGE(success, message);
}

BOOST_AUTO_TEST_SUITE_END()
