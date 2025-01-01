cmake_minimum_required(VERSION 4.0)
project(GraphLayout)

set(CMAKE_CXX_STANDARD 20)

add_library(GraphLayout STATIC
        include/graph_layout.h
        src/graph_layout.cpp
        include/hierarchical_layout.h
        include/feedback_arcs.h
        src/feedback_arcs.cpp
        include/graph_def.h
        src/graph_def.cpp
        include/layer_assignment.h
        src/layer_assignment.cpp
        src/graph_def_splitter.cpp
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
        tests/feedback_arcs/test_eades_93.cpp
        tests/layer_assignment/test_topological.cpp
        tests/graph_def/test_component_splitter.cpp
        tests/layer_assignment/test_network_simplex.cpp
)

target_link_libraries(runTests
        PRIVATE GraphLayout
        PRIVATE gtest gtest_main
)

add_test(NAME GraphLayoutTests COMMAND runTests)
