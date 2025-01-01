cmake_minimum_required(VERSION 4.0)
project(GraphLayout)

set(CMAKE_CXX_STANDARD 20)

add_library(GraphLayout STATIC
        include/graph_layout.h
        src/graph_layout.cpp
        include/feedback_arcs.h
        src/feedback_arcs.cpp
        include/graph_def.h
        src/graph_def.cpp
        include/layer_assignment.h
        src/layer_assignment.cpp
        src/graph_def_splitter.cpp
        include/cross_minimization.h
        src/cross_minimization.cpp
        include/binary_indexed_tree.h
        src/binary_indexed_tree.cpp
        include/vertex_positioning.h
        src/vertex_positioning.cpp
)

target_include_directories(GraphLayout
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

option(GRAPH_LAYOUT_BUILD_TESTS "Build tests" OFF)

if(GRAPH_LAYOUT_BUILD_TESTS)
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
            tests/layer_assignment/test_gansner_93.cpp
            tests/cross_minimization/test_cross_minimization.cpp
            tests/cross_minimization/test_barycenter.cpp
            tests/cross_minimization/test_median.cpp
            tests/cross_minimization/test_pairwise_switch.cpp
            tests/vertex_positioning/test_brandes_kopf.cpp
            tests/graph_layout/test_directed_graph_hierarchical_layout.cpp
    )

    target_link_libraries(runTests
            PRIVATE GraphLayout
            PRIVATE gtest gtest_main
    )

    add_test(NAME GraphLayoutTests COMMAND runTests)
endif ()
