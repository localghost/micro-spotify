#include <boost/test/unit_test.hpp>

#include <thread>
#include <base/task.h>

BOOST_AUTO_TEST_SUITE(TSTask)

// This two test cases test feature that most probably won't be implemented
BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(TCGetHandleMultipleException, 1)
BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(TCVoidGetHandleMultipleException, 1)

BOOST_AUTO_TEST_CASE(TCCreateTask)
{
  base::task<void> t{[]{ }};
}

BOOST_AUTO_TEST_CASE(TCExecuteTask)
{
  base::task<void> t{[]{ }};
  t();
}

BOOST_AUTO_TEST_CASE(TCGetHandle)
{
  base::task<void> t{[]{ }};
  t.get_handle();
}

BOOST_AUTO_TEST_CASE(TCGetHandleOfExecutedTask)
{
  base::task<void> t{[]{ }};
  t();
  t.get_handle();
}

BOOST_AUTO_TEST_CASE(TCGetResultOfNotExecutedTask)
{
  bool passed = false;

  base::task_handle<void> handle;
  {
    base::task<void> t{[]{ }};
    handle = t.get_handle();
  }

  try
  {
    handle.get();
  }
  catch (const base::task_error& e)
  {
    const base::task_error_code* code =
      boost::get_error_info<base::task_error_info>(e);
    if (code && (base::task_error_code::not_run == *code))
      passed = true;
  }

  BOOST_CHECK_EQUAL(passed, true);
}

BOOST_AUTO_TEST_CASE(TCReturnValueByArgument)
{
  int result = 0;
  base::task<void> t{[](int& result) { result = 42; }, std::ref(result)};
  t();
  t.get_handle().get();
  BOOST_CHECK_EQUAL(result, 42);
}

BOOST_AUTO_TEST_CASE(TCReturnValue)
{
  base::task<int> t{[] { return 42; }};
  t();
  int result = t.get_handle().get();
  BOOST_CHECK_EQUAL(result, 42);
}

BOOST_AUTO_TEST_CASE(TCReturnException)
{
  struct my_exception : std::exception {};

  bool exception_thrown = false;
  base::task<void> t{[] { throw my_exception{}; }};
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

BOOST_AUTO_TEST_CASE(TCCancel)
{
  base::task<void> t{[]{ }};
  t.get_handle().cancel();
}

BOOST_AUTO_TEST_CASE(TCExecuteCancelledTask)
{
  base::task<void> t{[]{ }};
  auto handle = t.get_handle();
  handle.cancel();
  t();
  handle.wait();
}

BOOST_AUTO_TEST_CASE(TCGetResultWhenCancelled)
{
  bool exception_thrown = true;
  base::task<int> t{[] { return 42; }};
  auto handle = t.get_handle();
  handle.cancel();
  t();
  try
  {
    handle.get();
  }
  catch (const base::task_error& e)
  {
    const base::task_error_code* code =
      boost::get_error_info<base::task_error_info>(e);
    if (code && (base::task_error_code::cancelled == *code))
      exception_thrown = true;
  }
  BOOST_CHECK_EQUAL(exception_thrown, true);
}

// why discard such functionality?
// maybe handle should be more like shared_future?
BOOST_AUTO_TEST_CASE(TCGetHandleMultipleException)
{
  bool exception_thrown = false;

  base::task<int> t{[] { return 42; }};
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

BOOST_AUTO_TEST_CASE(TCVoidGetHandleMultipleException)
{
  bool exception_thrown = false;

  base::task<void> t{[] { }};
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

BOOST_AUTO_TEST_CASE(TCExecuteOnDifferentThread)
{
  base::task<int> t{[] { return 42; }};
  auto handle = t.get_handle();

  std::thread th{std::move(t)};
  BOOST_CHECK_EQUAL(handle.get(), 42);
  th.join();
}

BOOST_AUTO_TEST_SUITE_END()
