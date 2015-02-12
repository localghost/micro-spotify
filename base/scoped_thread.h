#ifndef BASE_SCOPED_THREAD_H
#define BASE_SCOPED_THREAD_H

#include <thread>
#include <utility>
#include <base/compatibility.h>
#include <base/export.h>

namespace base {
class EXPORT_API scoped_thread FINAL
{
public:
  explicit scoped_thread(std::thread t);

  template<typename F, typename ...Args>
  explicit scoped_thread(F&& f, Args&&... args)
    : thread_(std::forward<F>(f), std::forward<Args>(args)...)
  { }

  ~scoped_thread();

  const std::thread& operator*() const;
  const std::thread* operator->() const;

private:
  std::thread thread_;
};
}

#endif // BASE_SCOPED_THREAD_H
