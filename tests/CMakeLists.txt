find_package(Boost REQUIRED COMPONENTS system unit_test_framework)

add_definitions("-DBOOST_TEST_DYN_LINK")

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

add_subdirectory(base)
add_subdirectory(engine)
