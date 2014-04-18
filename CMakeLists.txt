project(micro-spotify)

cmake_minimum_required(VERSION 2.8.5)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_BUILD")

if (${CMAKE_COMPILER_IS_GNUCXX})
    add_definitions("-std=c++0x")
else (${CMAKE_COMPILER_IS_GNUCXX})
    add_definitions("-std=c++11")
endif (${CMAKE_COMPILER_IS_GNUCXX})

add_definitions("-Wall")
add_definitions("-pedantic")
add_definitions("-Wextra")

find_package(PkgConfig)

set(TARGET_BASE "base")
set(TARGET_ENGINE "engine")
set(TARGET_CLIENT "client")

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(base)
add_subdirectory(engine)
#add_subdirectory(client)
add_subdirectory(tests)
