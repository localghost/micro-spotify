find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

set(HDRS
    log.h
    exception.h
    message_loop.h)

set(SRCS
    log.cc
    exception.cc
    message_loop.cc)

add_library(${TARGET_BASE} STATIC ${SRCS})
