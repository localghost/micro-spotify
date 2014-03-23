#ifndef ENGINE_CONFIGURATION_H
#define ENGINE_CONFIGURATION_H

#include <vector>
#include <functional>
#include <utility>
#include <boost/filesystem/path.hpp>
#include <boost/signals2.hpp>

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

FINAL struct configuration
{
    static configuration load(const boost::filesystem::path& path);
    static void save(const boost::filesystem::path& path);

    configuration_element<boost::filesystem::path> cache;
    configuration_element<std::string> username;
    // FIXME use when encrypted<string> is implemented
    // configuration_element<encrypted<string>> password;
    configuration_element<std::string> password;
};

class configurator

#endif
