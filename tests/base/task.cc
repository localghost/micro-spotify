#include <boost/test/unit_test.hpp>

#include <type_traits>
#include <thread>
#include <base/task.h>

BOOST_AUTO_TEST_SUITE(TSTask)

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

BOOST_AUTO_TEST_CASE(TCHandleValid)
{
  auto t = base::make_task([]{});
  auto handle = t.get_handle();
  BOOST_CHECK_EQUAL(handle.is_valid(), true);
}

BOOST_AUTO_TEST_CASE(TCHandleInvalid)
{
  base::task_handle<void> handle;
  BOOST_CHECK_EQUAL(handle.is_valid(), false);
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

  bool passed = false;
  base::task<void> t{[] { throw my_exception{}; }};
  t();
  try
  {
    t.get_handle().get();
  }
  catch (my_exception)
  {
    passed = true;
  }
  BOOST_CHECK_EQUAL(passed, true);
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
  bool passed = true;
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
      passed = true;
  }
  BOOST_CHECK_EQUAL(passed, true);
}

BOOST_AUTO_TEST_CASE(TCGetResultMultipleTimes)
{
  bool passed = true;
  base::task<int> t{[] { return 42; }};
  auto handle = t.get_handle();
  t();
  static_cast<void>(handle.get());
  try
  {
    handle.get();
  }
  catch (const base::task_error& e)
  {
    const base::task_error_code* code =
      boost::get_error_info<base::task_error_info>(e);
    if (code && (base::task_error_code::no_state == *code))
      passed = true;
  }
  BOOST_CHECK_EQUAL(passed, true);
}

BOOST_AUTO_TEST_CASE(TCGetHandleMultipleException)
{
  bool passed = false;

  base::task<int> t{[] { return 42; }};
  t.get_handle();
  try 
  {
    t.get_handle();
  }
  catch (base::task_error)
  {
    passed = true;
  }
  
  BOOST_CHECK_EQUAL(passed, true);
}

BOOST_AUTO_TEST_CASE(TCVoidGetHandleMultipleException)
{
  bool passed = false;

  base::task<void> t{[] { }};
  t.get_handle();
  try 
  {
    t.get_handle();
  }
  catch (base::task_error)
  {
    passed = true;
  }
  
  BOOST_CHECK_EQUAL(passed, true);
}

BOOST_AUTO_TEST_CASE(TCExecuteTaskMultipleTimes)
{
  bool passed = false;

  base::task<void> t{[]{}};
  t();
  try
  {
    t();
  }
  catch (const base::task_error& e)
  {
    const base::task_error_code* code =
      boost::get_error_info<base::task_error_info>(e);
    if (code && (base::task_error_code::already_called == *code))
      passed = true;
  }

  BOOST_CHECK_EQUAL(passed, true);
}

BOOST_AUTO_TEST_CASE(TCExecuteOnDifferentThread)
{
  base::task<int> t{[] { return 42; }};
  auto handle = t.get_handle();

  std::thread th{std::move(t)};
  BOOST_CHECK_EQUAL(handle.get(), 42);
  th.join();
}

BOOST_AUTO_TEST_CASE(TCMakeTask1)
{
  auto t = base::make_task([] {});
  BOOST_CHECK((std::is_same<decltype(t)::result_type, void>::value));
}

BOOST_AUTO_TEST_CASE(TCMakeTask2)
{
  auto t = base::make_task([] { return 42; });
  BOOST_CHECK((std::is_same<decltype(t)::result_type, int>::value));
}

BOOST_AUTO_TEST_CASE(TCMakeTask3)
{
  auto t = base::make_task([](int i) { return i; }, 42);
  BOOST_CHECK((std::is_same<decltype(t)::result_type, int>::value));

  t();
  BOOST_CHECK_EQUAL(t.get_handle().get(), 42);
}

BOOST_AUTO_TEST_CASE(TCMakeTask4)
{
  int i = 0;
  auto t = base::make_task([](int& i) { i = 42; }, std::ref(i));
  BOOST_CHECK((std::is_same<decltype(t)::result_type, void>::value));

  t();
  t.get_handle().wait();
  BOOST_CHECK_EQUAL(i, 42);
}

BOOST_AUTO_TEST_CASE(TCContinuation1)
{
  int counter = 0;
  auto inc_counter = [&counter]{++counter;};

  // TODO Use main thread when it is implemented
  base::thread th;
  th.start();
  auto t = base::make_task(inc_counter);
  auto handle = t.then(inc_counter).then(inc_counter).then(inc_counter).get_handle();
  th.post_task(std::move(t));
  handle.wait();
  th.stop();

  BOOST_CHECK_EQUAL(counter, 4);
}

BOOST_AUTO_TEST_SUITE_END()
