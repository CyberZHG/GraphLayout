cmake_minimum_required(VERSION 4.0)
project(demo)

set(CMAKE_CXX_STANDARD 20)

add_executable(demo
        main.cpp
)

include(FetchContent)

FetchContent_Declare(
        GraphLayout
        GIT_REPOSITORY https://github.com/CyberZHG/GraphLayout.git
        # GIT_REPOSITORY git@github.com:CyberZHG/GraphLayout.git
        GIT_TAG v0.5
)

FetchContent_MakeAvailable(GraphLayout)

target_link_libraries(demo PRIVATE GraphLayout)
