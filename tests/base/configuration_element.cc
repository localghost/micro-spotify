#include <boost/test/unit_test.hpp>

#include <base/configuration_element.h>

BOOST_AUTO_TEST_SUITE(TSBaseConfigurationElement)

BOOST_AUTO_TEST_CASE(TCBaseConfigurationElementNotified)
{
  bool pass = false;

  base::configuration_element<std::string> ces;
  ces.connect([&](const std::string&)
  {
    pass = true;
  });
  ces = "new value";

  BOOST_CHECK(pass);
}

BOOST_AUTO_TEST_CASE(TCBaseConfigurationElementNotifiedNewValue)
{
  bool pass = false;

  base::configuration_element<std::string> ces{"init value"};
  ces.connect([&](const std::string& new_value)
  {
    if ("new value" == new_value)
      pass = true;
  });

  BOOST_CHECK_EQUAL(ces.value(), "init value");

  ces = "new value";

  BOOST_CHECK(pass);
}

BOOST_AUTO_TEST_SUITE_END()
