set(TARGET_NAME "tests-base")

set(SRCS
    main.cc
    log.cc
    configuration_element.cc
#    callable.cc
#    thread_safe_queue.cc
    message_loop.cc
    thread.cc
    task.cc
    waitable_event.cc
    task_queue.cc
    callable.cc
    scoped_thread.cc
)

if (${CMAKE_HOST_APPLE})
include_directories("/opt/local/include")
link_directories("/opt/local/lib") # this should not be necessary!
endif (${CMAKE_HOST_APPLE})

add_executable(${TARGET_NAME} ${SRCS})
target_link_libraries(${TARGET_NAME}
                      ${TARGET_BASE}
                      ${Boost_LIBRARIES})
