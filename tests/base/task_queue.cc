#include <boost/test/unit_test.hpp>

#include <thread>
#include <atomic>
#include <random>
#include <vector>
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

// TODO Move this test case to a separate test suite (for stress tests)
/// STRESS
/// Tests whether task queue properly works in a multiple threads setup.
BOOST_AUTO_TEST_CASE(TCTaskQueue4)
{
  base::task_queue queue;
  std::atomic<unsigned> counter{0};
  std::atomic<bool> running{true};
  std::vector<std::thread> publishers;
  std::vector<std::thread> consumers;
  const unsigned num_of_threads = 100;

  for (unsigned i = 0; i < num_of_threads; ++i)
    consumers.emplace_back([&]
      {
        while (running)
        {
          auto result = queue.wait_and_pop();
          for (auto& t : result) t.action();
        }
      });

  for (unsigned i = 0; i < num_of_threads; ++i)
    publishers.emplace_back([&]
      {
        std::random_device device;
        std::mt19937 generator{device()};
        std::uniform_int_distribution<unsigned short> dist{0, 1000};

        auto inc_counter = [&counter]{++counter;};

        unsigned loop_counter = 42;
        while (running && loop_counter--)
        {
          queue.push(base::timed_task{inc_counter});
          queue.push(base::timed_task{inc_counter,
                                      base::high_steady_clock::now() + std::chrono::milliseconds{dist(generator)}});
        }
      });

  queue.push(base::timed_task{[&]{running = false;}, base::high_steady_clock::now() + 1000_ms});

  for (auto& p : publishers) p.join();
  for (auto& c : consumers) c.join();

  BOOST_CHECK(counter != 0);
}

BOOST_AUTO_TEST_SUITE_END()
