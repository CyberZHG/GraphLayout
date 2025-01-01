Graph Layout
============

[![C++ Unit Tests](https://github.com/CyberZHG/GraphLayout/actions/workflows/cpp-unit-test.yml/badge.svg)](https://github.com/CyberZHG/GraphLayout/actions/workflows/cpp-unit-test.yml)
[![Python Build & Test](https://github.com/CyberZHG/GraphLayout/actions/workflows/python-build-test.yml/badge.svg)](https://github.com/CyberZHG/GraphLayout/actions/workflows/python-build-test.yml)
[![WASM Build & Test](https://github.com/CyberZHG/GraphLayout/actions/workflows/wasm-build-test.yml/badge.svg)](https://github.com/CyberZHG/GraphLayout/actions/workflows/wasm-build-test.yml)
[![Deploy Docs to GitHub Pages](https://github.com/CyberZHG/GraphLayout/actions/workflows/deploy-docs.yml/badge.svg)](https://github.com/CyberZHG/GraphLayout/actions/workflows/deploy-docs.yml)
[![PyPI version](https://img.shields.io/pypi/v/sp-graph-layout.svg)](https://pypi.org/project/sp-graph-layout/)
[![npm version](https://img.shields.io/npm/v/sp-graph-layout.svg)](https://www.npmjs.com/package/sp-graph-layout)

A library for visualizing directed graphs. 

[Demo](./demo)

|         rank dir = TB          |         rank dir = LR          |
|:------------------------------:|:------------------------------:|
| ![](docs/images/example1_tb.svg) | ![](docs/images/example1_lr.svg) |
| ![](docs/images/example2_tb.svg) | ![](docs/images/example2_lr.svg) |

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
