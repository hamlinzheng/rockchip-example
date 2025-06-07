# Video Capture & RTSP Streaming Example

This project demonstrates basic video capture and RTSP streaming functionality on Rockchip platforms using OpenCV and GStreamer.

## Structure

- `main.cpp`: Main source file for video capture and RTSP streaming logic.
- `CMakeLists.txt`: Build configuration for CMake.
- `build.sh`: Shell script to build the project.

## Requirements

- Rockchip platform (e.g., RK3399Pro, RK3588)
- C++ compiler (g++)
- CMake
- OpenCV (with GStreamer support)
- GStreamer plugins (base, good, bad, mpp, rtsp)

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
./build/video_capture <v4l2_device> <rtsp_url> [width] [height] [fps]
```

- `<v4l2_device>`: e.g. `/dev/video0`
- `<rtsp_url>`: e.g. `rtsp://127.0.0.1:8554/live`
- `[width] [height] [fps]`: (optional) video resolution and frame rate, default 1280x720@30


## Note

```sh
## RGA (Raster Graphic Acceleration)
sudo watch -n 1 cat /sys/kernel/debug/rkrga/load

## MPP (Media Process Platform)
sudo watch -n 1 cat /proc/mpp_service/sessions-summary
```
