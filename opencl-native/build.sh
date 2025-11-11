#!/bin/bash

# Build script for OpenCL Native Vector Computation Example

# Set build directory
BUILD_DIR="build"

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory..."
    mkdir -p "$BUILD_DIR"
fi

# Navigate to build directory
cd "$BUILD_DIR" || exit 1

# Run CMake configuration
echo "Configuring project with CMake..."
cmake .. || {
    echo "CMake configuration failed!"
    exit 1
}

# Build the project
echo "Building project..."
make -j$(nproc) || {
    echo "Build failed!"
    exit 1
}

echo ""
echo "=========================================="
echo "Build completed successfully!"
echo "Executable: $BUILD_DIR/opencl_native"
echo "=========================================="
echo ""
echo "To run the program:"
echo "  cd $BUILD_DIR && ./opencl_native"
echo "Or:"
echo "  ./$BUILD_DIR/opencl_native"
echo ""
