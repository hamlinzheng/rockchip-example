#!/bin/bash

set -e

mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=Release ..

make -j$(nproc) VERBOSE=1

echo "Executable generated at: $(pwd)/video_capture_publish"
