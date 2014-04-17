#include <boost/test/unit_test.hpp>

#include <base/thread_safe_queue.h>
#include <base/log.h>

#include <thread>
#include <atomic>
#include <vector>
#include <functional>

namespace {
const unsigned num_of_threads = 50;
std::atomic<int> counter{0};
std::atomic<bool> can_go{false};
void thread_entry(base::thread_safe_queue<std::thread::id>& queue)
{
  while (!can_go) std::this_thread::yield();
  queue.push(std::this_thread::get_id());
  std::thread::id id;
  queue.pop(id);
  ++counter;
}
}

BOOST_AUTO_TEST_SUITE(TSBaseThreadSafeQueue)

BOOST_AUTO_TEST_CASE(TCBaseThreadSafeQueue)
{
  base::thread_safe_queue<std::thread::id> queue;
  std::vector<std::thread> threads;
  for (unsigned i = 0; i < num_of_threads; ++i)
    threads.push_back(std::thread{&thread_entry, std::ref(queue)});

  can_go = true;

  for (auto it = threads.begin(); it != threads.end(); ++it)
    it->join();

  BOOST_CHECK_EQUAL(counter, num_of_threads);
}

BOOST_AUTO_TEST_SUITE_END()
