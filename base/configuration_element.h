#ifndef BASE_CONFIGURATION_ELEMENT_H
#define BASE_CONFIGURATION_ELEMENT_H

#include <utility>
#include <mutex>
#include <boost/signals2.hpp>

#include <base/compatibility.h>

namespace base {
template<typename T>
class configuration_element FINAL
{
  typedef boost::signals2::signal<void(T)> signal_type;

public:
  typedef T value_type;
  typedef typename signal_type::slot_type slot_type;

  configuration_element() = default;

  configuration_element(T value) : value_(std::move(value)) { }

  T value() const
  {
    std::lock_guard<std::mutex> lock(mutex_);

    return value_;
  }

  void set_value(T value)
  {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      value_ = value;
    }
    on_change_(std::move(value));
  }

  boost::signals2::connection connect(const slot_type& slot)
  {
    return on_change_.connect(slot);
  }

private:
  T value_;
  mutable std::mutex mutex_;
  signal_type on_change_;
};
}

#endif
