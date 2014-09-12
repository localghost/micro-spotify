#include <boost/test/unit_test.hpp>

#include <mutex>
#include <condition_variable>
#include <chrono>

#include <base/exception.h>
#include <engine/configuration.h>
#include <engine/session.h>

BOOST_AUTO_TEST_SUITE(TSEngineSession)

// Tests whether engine::session object can be created.
BOOST_AUTO_TEST_CASE(TCEngineSessionCreate)
{
  bool success = true;
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

// Checks whether it is possible to log in into spotify via
// engine::session API.
BOOST_AUTO_TEST_CASE(TCEngineSessionLogIn)
{
  bool success = false;
  std::string message;
  std::mutex m;
  std::condition_variable cv;

  try 
  {
    engine::configuration config;
    engine::session s{config};
    s.connect_logged_in([&](sp_error err)
        {
          {
            std::lock_guard<std::mutex> guard{m};
            if (SP_ERROR_OK == err)
              success = true;
          }
          cv.notify_one();
        });
    s.log_in();
    std::unique_lock<std::mutex> guard{m};
    if (!cv.wait_for(guard, std::chrono::seconds{5}, [&]{ return success; }))
      message = "log in slot not called";
  }
  catch (base::exception)
  {
    success = false;
    message = boost::current_exception_diagnostic_information();
  }

  BOOST_CHECK_MESSAGE(success, message);
}

BOOST_AUTO_TEST_SUITE_END()
