Installation
============

## Python

Make sure you have `cmake` installed and then you can install this package with `pip`:

```bash
pip install sp-graph-layout
```

The `sp-` in the package name has no particular meaning; it is only there to avoid occupying the name for a better implementation.

## JavaScript

Install this package with `npm`:

```bash
npm i sp-graph-layout
```

The `sp-` in the package name has no particular meaning; it is only there to avoid occupying the name for a better implementation.

## CMake

You can use `FetchContent` to include this library:

```cmake
cmake_minimum_required(VERSION 4.0)
project(demo)

set(CMAKE_CXX_STANDARD 20)

add_executable(demo
        main.cpp
)

include(FetchContent)

FetchContent_Declare(
        SVGDiagram
        GIT_REPOSITORY https://github.com/CyberZHG/GraphLayout.git
        GIT_TAG main
)

FetchContent_MakeAvailable(SVGDiagram)

target_link_libraries(demo PRIVATE SVGDiagram)
```

### Optional Requirements

By default, this library estimates text dimensions using approximations. For more accurate measurements, PangoCairo can be enabled.

To install PangoCairo:

MacOS:

```bash
brew install cairo pango pkg-config
```

Linux:

```bash
sudo apt install libcairo2-dev libpango1.0-dev pkg-config
```

To enable PangoCairo, add the following to your CMake options:

```bash
-DSVG_DIAGRAM_ENABLE_PANGO_CAIRO=ON
```

### Development

To enable tests, add the following to your CMake options:

```bash
-DSVG_DIAGRAM_BUILD_TESTS=ON
```
