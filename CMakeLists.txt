cmake_minimum_required(VERSION 4.0)
project(GraphLayout)

set(CMAKE_CXX_STANDARD 20)

add_library(GraphLayout STATIC
        include/graph_layout.h
        src/graph_layout.cpp
)

target_include_directories(GraphLayout
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

enable_testing()

include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
)

FetchContent_MakeAvailable(googletest)

add_executable(runTests
        tests/test_main.cpp
)

target_link_libraries(runTests
        PRIVATE GraphLayout
        PRIVATE gtest gtest_main
)

add_test(NAME GraphLayoutTests COMMAND runTests)
