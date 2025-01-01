Graph Layout
============

A library for visualizing directed graphs. 

[Demo](./demo)

|        rankdir = TB         |        rankdir = LR         |
|:---------------------------:|:---------------------------:|
| ![](doc/img/example_tb.svg) | ![](doc/img/example_lr.svg) |

## Requirements

MacOS:

```bash
brew install cairo pango pkg-config
```

Linux:

```bash
sudo apt install libcairo2-dev libpango1.0-dev pkg-config
```

## Development

Add the following to CMake options to enable tests:

```bash
-DGRAPH_LAYOUT_BUILD_TESTS=ON
```
