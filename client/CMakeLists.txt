#pkg_check_modules(deps REQUIRED
#                  oauth
#                  libcrypto
#                  libcurl)

find_package(Boost REQUIRED)

include_directories(
    ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})

set(REQUIRED_LIBRARIES
    ${TARGET_BASE}
)

set(HDRS)

set(SRCS)

add_executable(${TARGET_CLIENT} ${SRCS} ${HDRS})
target_link_libraries(${TARGET_CLIENT} ${REQUIRED_LIBRARIES})
