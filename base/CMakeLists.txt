find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

set(HDRS
    exception.h
    log.h
    message_loop.h
    thread.h
    task.h
    waitable_event.h
  chrono.h
  threadpool.h
  scoped_thread.h
   )

set(SRCS
    exception.cc
    log.cc
    message_loop.cc
    thread.cc
    waitable_event.cc
    task_queue.cc
  scoped_thread.cc
   )

add_library(${TARGET_BASE} SHARED ${SRCS} ${HDRS})
set_target_properties(${TARGET_BASE} PROPERTIES COMPILE_FLAGS "-fPIC")
