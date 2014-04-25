pkg_search_module(libspotify REQUIRED libspotify)

include_directories(${libspotify_INCLUDE_DIRS})

find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

set(HDRS
    configuration.h
    exception.h
    session.h)

set(SRCS
    session.cc)

add_library(${TARGET_ENGINE} SHARED ${SRCS})
target_link_libraries(${TARGET_ENGINE} ${TARGET_BASE} ${libspotify_LIBRARIES})
