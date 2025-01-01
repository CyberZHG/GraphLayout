cmake_minimum_required(VERSION 4.0)
project(GraphLayout)

set(CMAKE_CXX_STANDARD 20)

if(APPLE)
    set(ENV{PKG_CONFIG_PATH} "/opt/homebrew/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
endif()

find_package(PkgConfig REQUIRED)

pkg_check_modules(CAIRO REQUIRED cairo)
include_directories(${CAIRO_INCLUDE_DIRS})
link_directories(${CAIRO_LIBRARY_DIRS})

pkg_check_modules(PANGO REQUIRED pango pangocairo)
include_directories(${PANGO_INCLUDE_DIRS})
link_directories(${PANGO_LIBRARY_DIRS})

add_library(GraphLayout STATIC
        include/graph_layout.h
        src/graph_layout.cpp
        include/directed/feedback_arcs.h
        src/directed/feedback_arcs.cpp
        include/common/graph_def.h
        src/common/graph_def.cpp
        include/directed/layer_assignment.h
        src/directed/layer_assignment.cpp
        src/common/graph_def_splitter.cpp
        include/directed/cross_minimization.h
        src/directed/cross_minimization.cpp
        include/common/binary_indexed_tree.h
        src/common/binary_indexed_tree.cpp
        include/directed/vertex_positioning.h
        src/directed/vertex_positioning.cpp
)

target_include_directories(GraphLayout
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(GraphLayout
        PUBLIC ${CAIRO_LIBRARIES}
        PUBLIC ${PANGO_LIBRARIES}
)

target_compile_options(GraphLayout PUBLIC ${CAIRO_CFLAGS_OTHER} ${PANGO_CFLAGS_OTHER})

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
