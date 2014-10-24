#include <boost/test/unit_test.hpp>

#include <thread>
#include <chrono>
#include <vector>
#include <base/waitable_event.h>

BOOST_AUTO_TEST_SUITE(TSBaseWaitableEvent)

/// Tests whether waitable event works for a single waiter.
/// Uses synchronisation via timeout to test only a single instance
/// of a @c waitable_event object.
BOOST_AUTO_TEST_CASE(TCBaseWaitableEvent1)
{
  unsigned counter = 0;
  base::waitable_event waitable;

  std::thread thread{[&]
    {
      waitable.wait();
      ++counter;
    }};
  // Assuring the thread has started before waitable is signaled.
  // I know, not the best approach (check the next test case).
  std::this_thread::sleep_for(std::chrono::milliseconds{500});
  waitable.signal();
  thread.join();

  BOOST_CHECK_EQUAL(counter, 1);
}

/// Tests whether waitable event works for a single waiter
/// Uses synchronisation via two instances of @c waitable_event.
BOOST_AUTO_TEST_CASE(TCBaseWaitableEvent2)
{
  unsigned counter = 0;
  base::waitable_event waitable_main;
  base::waitable_event waitable_thread;

  std::thread thread{[&]
    {
      waitable_main.signal();
      waitable_thread.wait();
      ++counter;
    }};
  waitable_main.wait();
  waitable_thread.signal();
  thread.join();

  BOOST_CHECK_EQUAL(counter, 1);
}

/// Tests whether calling @c signal() before @c wait() does not cause
/// the waiting thread to wait forever.
BOOST_AUTO_TEST_CASE(TCBaseWaitableEvent3)
{
  unsigned counter = 0;
  base::waitable_event waitable;

  waitable.signal();
  std::thread thread{[&]
    {
      waitable.wait();
      ++counter;
    }};
  thread.join();

  BOOST_CHECK_EQUAL(counter, 1);
}

/// Tests @c wait_and_reset() API
BOOST_AUTO_TEST_CASE(TCBaseWaitableEvent4)
{
  unsigned counter = 0;
  unsigned local_counter = counter;
  const unsigned expected = 42;
  base::waitable_event waitable;
  base::waitable_event fire_signal;

  std::thread thread{[&]
    {
      while (counter < expected)
      {
        fire_signal.signal();
        waitable.wait_and_reset();
        ++counter;
      }
      fire_signal.signal();
    }};
  while (local_counter < expected)
  {
    fire_signal.wait_and_reset();
    local_counter = counter;
    waitable.signal();
  }
  thread.join();

  BOOST_CHECK_EQUAL(counter, expected);
}

///// Tests whether waitable event works for multiple waiters
//BOOST_AUTO_TEST_CASE(TCBaseWaitableEvent5)
//{
//  unsigned counter = 0;
//  std::vector<std::thread> threads;
//
//  base::waitable_event waitable;
//  auto wait_on = [&] { waitable.wait(); ++counter; };
//
//  for (int i = 0; i < 10; ++i) threads.emplace_back(wait_on);
//
//  waitable.signal();
//
//  for (auto& t : threads) t.join();
//
//  BOOST_CHECK_EQUAL(counter, threads.size());
//}

BOOST_AUTO_TEST_SUITE_END()
