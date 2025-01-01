#!/usr/bin/env bash

emcmake cmake .. -DGRAPH_LAYOUT_ENABLE_SVG=OFF -DGRAPH_LAYOUT_BUILD_WEB_ASSEMBLY=ON -DCMAKE_BUILD_TYPE=Release
emmake make GraphLayoutWASM
