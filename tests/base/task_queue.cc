#include <boost/test/unit_test.hpp>

#include <thread>
#include <base/chrono.h>
#include <base/task_queue.h>

BOOST_AUTO_TEST_SUITE(TSBaseTaskQueue)

/// Pushes a simple @c timed_task on a queue then pops it
/// and executes the @c action
/// Works on a single thread.
BOOST_AUTO_TEST_CASE(TCTaskQueue1)
{
  base::task_queue queue;
  unsigned counter = 0;

  queue.push(base::timed_task([&counter]{++counter;}));
  queue.wait_and_pop().at(0).action();

  BOOST_CHECK_EQUAL(counter, 1);
}

/// Pushes a simple @c timed_task with a delay on a queue then pops it
/// and executes the @c action
/// Works on a single thread.
BOOST_AUTO_TEST_CASE(TCTaskQueue2)
{
  base::task_queue queue;
  unsigned counter = 0;

  queue.push(base::timed_task([&counter]{++counter;},
                              base::high_steady_clock::now() + 100_ms));
  queue.wait_and_pop().at(0).action();

  BOOST_CHECK_EQUAL(counter, 1);
}

/// Tests whether task queue properly works in a multiple threads setup.
/// Pops and executes tasks in the loop until one of the tasks pushed
/// on a different thread sets the loop variable to false.
BOOST_AUTO_TEST_CASE(TCTaskQueue3)
{
  base::task_queue queue;
  unsigned counter = 0;
  bool running = true;

  std::thread publisher{[&]
    {
      queue.push(base::timed_task([&counter]{++counter;}));
      std::this_thread::sleep_for(300_ms);
      queue.push(base::timed_task([&counter, &running]
                                  {
                                    ++counter;
                                    running = false;
                                  },
                                  base::high_steady_clock::now() + 200_ms));
    }};

  while (running)
  {
    auto result = queue.wait_and_pop();
    for (auto& t : result) t.action();
  }

  publisher.join();

  BOOST_CHECK_EQUAL(counter, 2);
}

BOOST_AUTO_TEST_SUITE_END()
