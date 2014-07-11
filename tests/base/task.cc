#include <boost/test/unit_test.hpp>

#include <thread>
#include <base/task.h>

BOOST_AUTO_TEST_SUITE(TSBaseTask)

BOOST_AUTO_TEST_CASE(TCBaseTask)
{
  int result = 0;
  base::task<void()> t{[&result] { result = 42; }};
  t();
  t.get_handle().wait();
  BOOST_CHECK_EQUAL(result, 42);
}

BOOST_AUTO_TEST_CASE(TCBaseTaskResult)
{
  base::task<int()> t{[] { return 42; }};
  t();
  int result = t.get_handle().get();
  BOOST_CHECK_EQUAL(result, 42);
}

BOOST_AUTO_TEST_CASE(TCBaseTaskException)
{
  struct my_exception : std::exception {};

  bool exception_thrown = false;
  base::task<void()> t{[] { throw my_exception{}; }};
  t();
  try
  {
    t.get_handle().get();
  }
  catch (my_exception)
  {
    exception_thrown = true;
  }
  BOOST_CHECK_EQUAL(exception_thrown, true);
}

BOOST_AUTO_TEST_CASE(TCBaseTaskCancel)
{
  base::task<int()> t{[] { return 42; }};
  auto handle = t.get_handle();
  handle.cancel();
  t();
  handle.wait();
}

BOOST_AUTO_TEST_CASE(TCBaseTaskGetResultWhenCancelled)
{
  bool exception_thrown = false;

  base::task<int()> t{[] { return 42; }};
  auto handle = t.get_handle();
  handle.cancel();
  t();
  try 
  {
    handle.get();
  }
  catch (base::task_error)
  {
    exception_thrown = true;
  }
  
  BOOST_CHECK_EQUAL(exception_thrown, true);
}

// why discard such functionality?
BOOST_AUTO_TEST_CASE(TCBaseTaskGetHandleMultipleException)
{
  bool exception_thrown = false;

  base::task<int()> t{[] { return 42; }};
  t.get_handle();
  try 
  {
    t.get_handle();
  }
  catch (base::task_error)
  {
    exception_thrown = true;
  }
  
  BOOST_CHECK_EQUAL(exception_thrown, true);
}

BOOST_AUTO_TEST_CASE(TCBaseTaskVoidGetHandleMultipleException)
{
  bool exception_thrown = false;

  base::task<void()> t{[] { }};
  t.get_handle();
  try 
  {
    t.get_handle();
  }
  catch (base::task_error)
  {
    exception_thrown = true;
  }
  
  BOOST_CHECK_EQUAL(exception_thrown, true);
}

BOOST_AUTO_TEST_CASE(TCBaseTaskThread)
{
  base::task<int()> t{[] { return 42; }};
  auto handle = t.get_handle();

  std::thread th{std::move(t)};
  BOOST_CHECK_EQUAL(handle.get(), 42);
  th.join();
}

BOOST_AUTO_TEST_SUITE_END()
