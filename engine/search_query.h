#ifndef ENGINE_SEARCH_QUERY_H
#define ENGINE_SEARCH_QUERY_H

#include <string>
#include <boost/optional.h>

namespace engine {
enum struct query_type
{
    All,
    Track,
    Album,
    Artist
};

struct search_query
{
    static search_query from_string(const std::string& query);

    query_type type;
    std::string value;
};
}

#endif
