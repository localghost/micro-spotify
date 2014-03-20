#include "search_query.h"

#include <boost/algorithm/string/trim.hpp>

namespace engine {
namespace {
std::string get_query_specifier_value(const std::string& query,
                                      const std::string& specifier)
{
    auto pos = query.find(specifier);
    if (pos == std::string::npos)
        return "";

    return query.substr(pos + specifier.size());
}
}

search_query search_query::from_string(std::string query)
{
    boost::algorithm::trim(query);

    search_query result;

    result.value = get_query_specifier_value(query, "t:");
    if (!result.value.empty())
    {
        result.type = query_type::Track;
        return result;
    }

    result.value = get_query_specifier_value(query, "a:");
    if (!result.value.empty())
    {
        result.type = query_type::Artist;
        return result;
    }

    result.value = get_query_specifier_value(query, "al:");
    if (!result.value.empty())
    {
        result.type = query_type::Album;
        return result;
    }

    result.value = query;
    result.type = All;
    return result;
}
}
