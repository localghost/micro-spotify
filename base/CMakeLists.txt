find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

set(HDRS
    log.h
    exception.h
    message_loop.h
    thread.h)

set(SRCS
    log.cc
    exception.cc
    message_loop.cc
    thread.cc)

add_library(${TARGET_BASE} SHARED ${SRCS})
set_target_properties(${TARGET_BASE} PROPERTIES COMPILE_FLAGS "-fPIC")
