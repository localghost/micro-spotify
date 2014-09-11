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

#define EXCEPTION_TYPE(ClassName) \
        struct ClassName : ::base::exception { }

// deprecated: use THROW instead
//#define EXCEPTION(ClassName) \
//        ::boost::enable_error_info(ClassName{}) \
//            << ::boost::throw_file{__FILE__} \
//            << ::boost::throw_line{int(__LINE__)} \
//            << ::boost::throw_function{BOOST_CURRENT_FUNCTION}

#define EXCEPTION_LOCATION \
            ::boost::throw_file{__FILE__} \
            << ::boost::throw_line{int(__LINE__)} \
            << ::boost::throw_function{BOOST_CURRENT_FUNCTION}

#define THROW(ExceptionObject) \
        ::boost::throw_exception(ExceptionObject << EXCEPTION_LOCATION)

//#define THROW(ClassName) ::boost::throw_exception(EXCEPTION(ClassName))

//// FIXME Add some prefix
//#define THROW_MESSAGE(ClassName, Message) \
//        do { \
//            ::std::ostringstream oss; \
//            oss << Message; \
//            ::boost::throw_exception( \
//                EXCEPTION(ClassName) \
//                    << ::base::error_info::message{oss.str()}); \
//        } while (false)

namespace base {
/// Exception resulted from an I/O operation
EXCEPTION_TYPE(io_error);
}

#endif
