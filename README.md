Graph Layout
============

A library for visualizing directed graphs. 

[Demo](./demo)

|         rank dir = TB          |         rank dir = LR          |
|:------------------------------:|:------------------------------:|
| ![](doc/image/example1_tb.svg) | ![](doc/image/example1_lr.svg) |
| ![](doc/image/example2_tb.svg) | ![](doc/image/example2_lr.svg) |

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
-DGRAPH_LAYOUT_ENABLE_TESTS=ON
```
