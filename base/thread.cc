#include "thread.h"

#include "assert.h"
#include "log.h"
#include "compatibility.h"

namespace base {
namespace {
THREAD_LOCAL thread* current_thread = nullptr;

// FIXME Only o mock up for the main thread
class main_thread : public base::thread
{
public:
  main_thread() { current_thread = this; }
  void start() { }
  void stop() { }
private:
  static main_thread thread_;
};
main_thread main_thread::thread_;
}

thread* thread::current()
{
  return current_thread;
}

thread::~thread()
{
  assert(!thread_.joinable());
}

void thread::start()
{
  // FIXME NOT thread-safe
  if (thread_.joinable()) return;

  thread_ = std::thread{&thread::exec, this};

  waiter_.wait();
}

void thread::stop()
{
  if (!thread_.joinable()) return;

  loop_.stop();
  thread_.join();
}

std::thread::id thread::id() const
{
  return thread_.get_id();
}

void thread::post_task(callable action, time_delay delay)
{
  BOOST_ASSERT(thread_.joinable());
  loop_.post_task(std::move(action), delay);
}

void thread::exec()
{
  current_thread = this;
  waiter_.signal();
  loop_.start();
}
}
