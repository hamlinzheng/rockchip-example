#!/bin/bash

# Color definitions
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}OpenCV + GStreamer Camera Build Script${NC}"
echo -e "${GREEN}========================================${NC}"

# Get script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_DIR="${SCRIPT_DIR}/build"

# Clean option
if [ "$1" == "clean" ]; then
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf "${BUILD_DIR}"
    echo -e "${GREEN}Clean completed!${NC}"
    exit 0
fi

# Create and enter build directory
echo -e "${YELLOW}Creating build directory: ${BUILD_DIR}${NC}"
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}" || exit 1

# Run CMake
echo -e "${YELLOW}Running CMake configuration...${NC}"
cmake .. -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    echo -e "${RED}CMake configuration failed!${NC}"
    exit 1
fi

# Build
echo -e "${YELLOW}Starting build...${NC}"
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed!${NC}"
    exit 1
fi

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Build successful!${NC}"
echo -e "${GREEN}========================================${NC}"
echo -e "${YELLOW}Executable location: ${BUILD_DIR}/camera_gstreamer${NC}"
echo ""
echo -e "${YELLOW}Usage:${NC}"
echo -e "  1. Default parameters (device: /dev/video0, resolution: 1920x1080, fps: 30):"
echo -e "     ${GREEN}./build/camera_gstreamer${NC}"
echo ""
echo -e "  2. Custom parameters:"
echo -e "     ${GREEN}./build/camera_gstreamer [device] [width] [height] [fps] [queue_size]${NC}"
echo -e "     Example: ${GREEN}./build/camera_gstreamer /dev/video0 640 480 30 5${NC}"
echo ""
echo -e "  3. Press 'q' or 'ESC' to exit the program"
echo ""
echo -e "${YELLOW}Tips:${NC}"
echo -e "  - Use ${GREEN}v4l2-ctl --list-devices${NC} to list available camera devices"
echo -e "  - Use ${GREEN}v4l2-ctl --device=/dev/video0 --list-formats-ext${NC} to check supported formats and resolutions"
echo -e "  - To clean: ${GREEN}./build.sh clean${NC}"
echo -e "${GREEN}========================================${NC}"
