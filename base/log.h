#ifndef BASE_LOG_H
#define BASE_LOG_H

#include <ostream>
#include <sstream>

namespace base {
namespace log {
enum class severity
{
    Info,
    Debug,
    Warning,
    Error
};

class message
{
public:
    message(severity severity_,
            const char* file,
            unsigned int line,
            const char* function);

    std::ostream& stream();

    ~message();
private:
    std::ostringstream oss;
};
}
}
 
#define LOG(Severity) \
    ::base::log::message{Severity, __FILE__, __LINE__, __FUNCTION__}.stream()

// If Condition is false then the branch with the Stream
// will not be evaluated.
// Relies on the std::ostream operator bool() but its result
// is not important in this case. It is only used as it is
// the simplest way to convert the expression with Stream
// to bool.
// TODO Verify that neither GCC nor Clang prints warning
//      like "value computed is not used" or similar
#define LOG_IF(Condition, Severity) \
    (Condition) && LOG(Severity)

#define LOG_INFO LOG(::base::log::severity::Info)
        
#ifdef DEBUG_BUILD
#define LOG_DEBUG LOG_IF(true, ::base::log::severity::Debug)
#else
// FIXME prints warning in GCC 4.6
#define LOG_DEBUG LOG_IF(false, ::base::log::severity::Debug)
#endif

#define LOG_WARNING LOG(::base::log::severity::Warning)

#define LOG_ERROR LOG(::base::log::severity::Error)
        
#endif
