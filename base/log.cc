#include "log.h"

#include <mutex>
#include <iostream>

namespace base {
namespace log {
namespace {
const char* severity_to_string(severity severity_)
{
  switch (severity_)
  {
    case severity::Info: return "INFO";
    case severity::Debug: return "DEBUG";
    case severity::Warning: return "WARNING";
    case severity::Error: return "ERROR";
  }
  return ""; // to surpress warnings
}
}

ostream_voidifier voidifier{};

message::message(severity severity_,
    const char* file,
    unsigned int line,
    const char* function)
{
  oss << "["
    << severity_to_string(severity_) << ":"
    // FIXME cut down the file to relative path
    << file << ":"
    << line << ":"
    << function
    << "] ";
}

std::ostream& message::stream()
{
  return oss;
}

message::~message()
{
  static std::mutex m;
  std::lock_guard<std::mutex> guard{m};
  // TODO check if operator<<() does not throw
  // FIXME lock for thread-safety
  std::cout << oss.str() << std::endl;
}
}
}
