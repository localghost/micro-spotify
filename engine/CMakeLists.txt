pkg_search_module(libspotify REQUIRED libspotify)

include_directories(${libspotify_INCLUDE_DIRS})

find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

set(HDRS
    configuration.h
    exception.h
    session.h
    global_threads_manager.h
   )

set(SRCS
    session.cc
    global_threads_manager.cc
   )

add_library(${TARGET_ENGINE} SHARED ${SRCS})
set_target_properties(${TARGET_ENGINE} PROPERTIES COMPILE_FLAGS "-fPIC")
target_link_libraries(${TARGET_ENGINE} ${TARGET_BASE} ${libspotify_LIBRARIES})
