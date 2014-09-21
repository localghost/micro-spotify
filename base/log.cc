#include "log.h"

#include <cstdio>
#include <mutex>
#include <iostream>

#include "thread.h"
#include "chrono.h"

namespace base {
namespace log {
namespace {
std::mutex m;

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
    // TODO add thread's id
//    << thread::current()->id() << ":"
    << high_steady_clock::now().time_since_epoch().count() << "|"
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
  try
  {
    std::lock_guard<std::mutex> guard{m};
    // reasoning for std::cerr:
    // - unbuffered by default on Linux
    // - there might be a terminal-based client
    std::cerr << oss.str() << std::endl;
  }
  catch (...)
  {
    std::fprintf(stderr, "%s:%d %s", __FILE__, __LINE__, __FUNCTION__);
  }
}
}
}
