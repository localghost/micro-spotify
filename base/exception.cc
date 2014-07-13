#include "exception.h"

#include <cstdlib>
#include <iostream>

namespace base {
void verbose_terminate_handler()
{
    if (std::current_exception())
    {
        try
        {
            throw;
        }
        catch (const exception& e)
        {
            std::cerr << boost::diagnostic_information(e) << std::endl;
        }
        catch (const boost::exception& e)
        {
            std::cerr << boost::diagnostic_information(e) << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
        catch (...)
        {
            std::cerr << "unknown exception" << std::endl;
        }
    }
    std::abort();
}
}
