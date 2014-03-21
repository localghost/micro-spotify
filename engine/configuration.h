#ifndef ENGINE_CONFIGURATION_H
#define ENGINE_CONFIGURATION_H

#include <vector>
#include <functional>
#include <utility>
#include <boost/filesystem/path.hpp>

template<typename T>
class configuration_item
{
public:
    typedef T value_type;

    configuration_item() = default;

    configuration_item(T value) : value(std::move(value)) { }

    T value() const { return value; }

    void set_value(T value)
    {
        this->value = std::move(value);
        for (const auto& observer : observers)
            observer(this->value);
    }

    template<typename ObserverType>
    void add_observer(ObserverType observer)
    {
        observers.push_back(std::move(observer));
    }

private:
    T value;
    std::vector<std::function<void(configuration_item)>> observers;
};

struct configuration
{
public:
    static configuration from_file(const boost::filesystem::path& path);

    boost::filesystem::path cache;
    std::string username;
    encrypted_string password;
};

#endif
