#ifndef BASE_EXCEPTION_H
#define BASE_EXCEPTION_H

#include <string>
#include <sstream>
#include <exception>
#include <boost/exception/all.hpp>
#include <base/inspect.h>

namespace base {
namespace error_info {
#if SUPPORTED(CPP11)
using message = boost::error_info<struct error_info_message_, std::string>;
#else
typedef boost::error_info<struct error_info_message_, std::string> message;
#endif
}

struct exception : virtual std::exception, virtual boost::exception {};

void verbose_terminate_handler();
}

#define MAKE_EXCEPTION_TYPE(ClassName) \
        struct ClassName : virtual ::base::exception { }

#define MAKE_EXCEPTION(ClassName) \
        ::boost::enable_error_info(ClassName{}) \
            << ::boost::throw_file{__FILE__} \
            << ::boost::throw_line{int(__LINE__)} \
            << ::boost::throw_function{BOOST_CURRENT_FUNCTION}

#define THROW(ClassName) ::boost::throw_exception(MAKE_EXCEPTION(ClassName))

// FIXME Add some prefix
#define THROW_MESSAGE(ClassName, Message) \
        do { \
            ::std::ostringstream oss; \
            oss << Message; \
            ::boost::throw_exception( \
                MAKE_EXCEPTION(ClassName) \
                    << ::base::error_info::message{oss.str()}); \
        } while (false)

namespace base {
/// Exception resulted from an I/O operation
MAKE_EXCEPTION_TYPE(io_error);
}

#endif
