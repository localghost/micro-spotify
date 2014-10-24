#ifndef BASE_TASK_QUEUE_H
#define BASE_TASK_QUEUE_H

#include <condition_variable>
#include <mutex>
#include <vector>
#include <base/timed_task.h>
#include <base/export.h>

namespace base {
class EXPORT_API task_queue
{
public:
  // TODO utilise perfect fwd?
  void push(timed_task t);

  std::vector<timed_task> wait_and_pop();

  void clear();

private:
  std::vector<timed_task> queue_;
  std::mutex lock_;
  std::condition_variable cv_;
};
}

#endif
