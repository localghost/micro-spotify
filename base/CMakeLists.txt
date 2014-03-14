find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${Boost_LIBRARY_DIRS})

set(HDRS
    log.h
    exception.h)

set(SRCS
    log.cc
    exception.cc)

add_library(${TARGET_BASE} STATIC ${SRCS})
