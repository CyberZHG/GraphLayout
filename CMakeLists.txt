cmake_minimum_required(VERSION 4.0)
project(GraphLayout)

set(CMAKE_CXX_STANDARD 20)

option(GRAPH_LAYOUT_ENABLE_SVG "Enable exporting to SVG" ON)

if(APPLE)
    set(ENV{PKG_CONFIG_PATH} "/opt/homebrew/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
endif()

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
        include/common/draw_svg.h
        src/common/draw_svg.cpp
        include/common/graph_attributes.h
        src/common/graph_attributes.cpp
)

target_include_directories(GraphLayout
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (GRAPH_LAYOUT_ENABLE_SVG)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(CAIRO REQUIRED cairo)
    pkg_check_modules(PANGO REQUIRED pango pangocairo)

    target_include_directories(GraphLayout
            PRIVATE ${CAIRO_INCLUDE_DIRS}
            PRIVATE ${PANGO_INCLUDE_DIRS}
    )

    target_link_directories(GraphLayout
            PUBLIC ${CAIRO_LIBRARY_DIRS}
            PUBLIC ${PANGO_LIBRARY_DIRS}
    )

    target_link_libraries(GraphLayout
            PUBLIC ${CAIRO_LIBRARIES}
            PUBLIC ${PANGO_LIBRARIES}
    )

    target_compile_options(GraphLayout
            PUBLIC ${CAIRO_CFLAGS_OTHER}
            PUBLIC ${PANGO_CFLAGS_OTHER})

    target_compile_definitions(GraphLayout
            PUBLIC GRAPH_LAYOUT_ENABLE_SVG
    )
endif()

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
            tests/feedback_arcs/test_min_id.cpp
            tests/svg/test_text.cpp
    )

    target_link_libraries(runTests
            PRIVATE GraphLayout
            PRIVATE gtest gtest_main
    )

    add_test(NAME GraphLayoutTests COMMAND runTests)
endif ()

option(GRAPH_LAYOUT_BUILD_WEB_ASSEMBLY "Compiling for WebAssembly" OFF)

if (GRAPH_LAYOUT_BUILD_WEB_ASSEMBLY)

    add_executable(GraphLayoutWASM
            js/graph_layout_js.cpp)

    target_link_libraries(GraphLayoutWASM
            PRIVATE GraphLayout
    )

    target_compile_options(GraphLayoutWASM PRIVATE "-O3")

    set_target_properties(GraphLayoutWASM PROPERTIES SUFFIX ".js")

    target_link_options(GraphLayoutWASM PRIVATE
            "--bind"
            "-sMODULARIZE=1"
            "-sEXPORT_ES6=1"
            "-sEXPORT_NAME=GraphLayoutWASMModule"
    )
endif ()
