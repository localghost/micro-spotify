#ifndef BASE_EXCEPTION_H
#define BASE_EXCEPTION_H

#include <string>
#include <exception>
#include <boost/exception/all.hpp>
#include <base/export.h>

namespace base {
namespace error_info {
using message = boost::error_info<struct error_info_message_, std::string>;
}

struct EXPORT_API exception : virtual std::exception, virtual boost::exception {};

void verbose_terminate_handler();
}

#define EXCEPTION_TYPE(ClassName) \
        struct ClassName : ::base::exception { }

#define EXCEPTION_LOCATION \
            ::boost::throw_file{__FILE__} \
            << ::boost::throw_line{int(__LINE__)} \
            << ::boost::throw_function{BOOST_CURRENT_FUNCTION}

#define THROW(ExceptionObject) \
        ::boost::throw_exception(ExceptionObject << EXCEPTION_LOCATION)

namespace base {
/// Exception resulted from an I/O operation
EXCEPTION_TYPE(io_error);

/// Exception resoulted from referencing beyond a valid range
EXCEPTION_TYPE(out_of_bounds_error);
}

#endif
