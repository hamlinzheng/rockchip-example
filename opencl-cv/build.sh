#!/bin/bash

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake ..

# Build
make -j$(nproc)

echo ""
echo "Build complete! Run with: ./build/check_opencv"
