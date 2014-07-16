#ifndef BASE_MESSAGE_LOOP_H
#define BASE_MESSAGE_LOOP_H

#include <vector>
#include <condition_variable>
#include <mutex>
#include <base/task.h>
#include <base/chrono.h>

namespace base {
// Tasks will be executed on the thread the start() method was invoked on.
class message_loop FINAL
{
private:
  struct task_model_base
  {
    virtual ~task_model_base() = default;
    virtual void call() = 0;
  };

  template<typename R>
  class task_model : public task_model_base
  {
  public:
    explicit task_model(task<R>&& t) : task_(std::move(t)) {}

    void call()
    {
      task_();
    }

  private:
    task<R> task_;
  };

  struct queued_task
  {
    queued_task(std::unique_ptr<task_model_base>&& t,
                high_steady_clock::time_point w)
      : task_(std::move(t)),
        when(w)
    { }

    friend bool operator<(const queued_task& x, const queued_task& y)
    {
      return x.when > y.when;
    }

    std::unique_ptr<task_model_base> task_;
    high_steady_clock::time_point when;
  };

public:
  void start();
  void stop();

  template<typename R>
  void queue_task(task<R> task_,
                  std::chrono::milliseconds delay = std::chrono::milliseconds{0})
  {
    std::unique_ptr<task_model_base> t{new task_model<R>{std::move(task_)}};
    queue_task_(std::move(t), delay);
  }

private:
  void exec();

  void queue_task_(std::unique_ptr<task_model_base>&& t,
                   std::chrono::milliseconds delay);

  bool active_ = false;

  // TODO Rewrite to use lock-free priority queue
  std::vector<queued_task> queue_;
  std::mutex mutex_;
  std::condition_variable waiter_;

  high_steady_clock::time_point next_loop_time_;
};
}

#endif
