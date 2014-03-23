#ifndef BASE_CONFIGURATION_ELEMENT_H
#define BASE_CONFIGURATION_ELEMENT_H

#include <utility>
#include <boost/signals2.hpp>

#include <base/compatibility.h>

template<typename T>
FINAL class configuration_element
{
    typedef boost::signals2::signal<void(const T&)> signal_type;

public:
    typedef T value_type;
    typedef signal_type::slot_type slot_type;

    configuration_element() = default;

    configuration_element(T value) : value_(std::move(value)) { }

    T value() const { return value_; }
    void set_value(T value)
    {
        value_ = std::move(value);
        on_value_change_(value_);
    }

    boost::signals2::scoped_connection connect(const slot_type& slot)
    {
        return on_value_change_.connect(slot);
    }

private:
    T value_;
    signal_type on_value_change_;
};

#endif
