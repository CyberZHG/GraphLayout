#!/usr/bin/env bash

emcmake cmake .. -B wasm -DGRAPH_LAYOUT_ENABLE_SVG=OFF -DGRAPH_LAYOUT_BUILD_WEB_ASSEMBLY=ON
(cd wasm && emmake make GraphLayoutWASM)
