set(TARGET_NAME "tests-base")

set(SRCS
    test_main.cc
    test_log.cc
    test_configuration_element.cc
    test_callable.cc
#    test_exception.cc
)

if (${CMAKE_HOST_APPLE})
include_directories("/opt/local/include")
link_directories("/opt/local/lib") # this should not be necessary!
endif (${CMAKE_HOST_APPLE})

add_executable(${TARGET_NAME} ${SRCS})
target_link_libraries(${TARGET_NAME}
                      ${TARGET_BASE}
                      ${Boost_LIBRARIES})
