# Video Capture Example

This project demonstrates basic video capture functionality on Rockchip platforms.

## Structure

- `main.cpp`: Main source file for video capture logic.
- `CMakeLists.txt`: Build configuration for CMake.
- `build.sh`: Shell script to build the project.

## Requirements

- Rockchip platform (e.g., RK3399Pro, RK3588)
- C++ compiler (g++)
- CMake

## Build

```sh
./build.sh
```

Or manually:

```sh
mkdir -p build
cd build
cmake ..
make
```

## Run

```sh
./build/video_capture
```
