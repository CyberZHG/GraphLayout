#!/usr/bin/env bash

emcmake cmake .. -B wasm -DGRAPH_LAYOUT_BIND_ES=ON
(cd wasm && emmake make GraphLayoutWASM)
