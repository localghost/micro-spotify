set(TARGET_NAME "tests-engine")

set(SRCS
    main.cc
    session.cc
)

if (${CMAKE_HOST_APPLE})
include_directories("/opt/local/include")
link_directories("/opt/local/lib") # this should not be necessary!
endif (${CMAKE_HOST_APPLE})

add_executable(${TARGET_NAME} ${SRCS})
target_link_libraries(${TARGET_NAME}
                      ${TARGET_BASE}
                      ${TARGET_ENGINE}
                      ${Boost_LIBRARIES})
