#ifndef ENGINE_CONFIGURATION_H
#define ENGINE_CONFIGURATION_H

#include <boost/filesystem/path.hpp>

#include <base/compatibility.h>

namespace engine {
struct configuration FINAL
{
    // TODO maybe it should load/save from/to a stream?
    static configuration load(const boost::filesystem::path& path);
    static void save(const boost::filesystem::path& path);

    base::configuration_element<boost::filesystem::path> cache;
    base::configuration_element<std::string> username;
    // FIXME use when encrypted<string> is implemented
    // configuration_element<encrypted<string>> password;
    base::configuration_element<std::string> password;
};
}

#endif
