# OpenCV + GStreamer + V4L2 Multi-threaded Camera Stream Example

## Project Overview

This is a V4L2 camera stream processing example based on OpenCV, GStreamer, and multi-threading technology. The program uses two independent threads:
- **Capture Thread**: Captures video frames from V4L2 camera
- **Display Thread**: Displays captured video frames

The two threads communicate through a thread-safe frame queue, enabling efficient video stream processing.

## Features

- ✅ Uses GStreamer as backend to capture V4L2 camera stream
- ✅ Multi-threaded architecture (capture and display separation)
- ✅ Thread-safe frame queue implementation
- ✅ Real-time FPS display (capture FPS and display FPS)
- ✅ Queue size monitoring
- ✅ Configurable parameters (device, resolution, frame rate, etc.)
- ✅ Graceful exit mechanism

## System Requirements

### Dependencies

1. **OpenCV** (>= 3.0, recommended 4.x)
   - Must be compiled with GStreamer support

2. **GStreamer** (>= 1.0)
   ```bash
   sudo apt-get install \
       libgstreamer1.0-dev \
       libgstreamer-plugins-base1.0-dev \
       libgstreamer-plugins-bad1.0-dev \
       gstreamer1.0-plugins-base \
       gstreamer1.0-plugins-good \
       gstreamer1.0-plugins-bad \
       gstreamer1.0-plugins-ugly \
       gstreamer1.0-libav \
       gstreamer1.0-tools \
       gstreamer1.0-x \
       gstreamer1.0-alsa \
       gstreamer1.0-gl \
       gstreamer1.0-gtk3 \
       gstreamer1.0-pulseaudio
   ```

3. **V4L2 Tools** (optional, for debugging)
   ```bash
   sudo apt-get install v4l-utils
   ```

4. **CMake** (>= 3.10)
   ```bash
   sudo apt-get install cmake
   ```

5. **Build Tools**
   ```bash
   sudo apt-get install build-essential pkg-config
   ```

### Verify OpenCV GStreamer Support

Run the following command to check if OpenCV supports GStreamer:

```bash
python3 -c "import cv2; print(cv2.getBuildInformation())" | grep -i gstreamer
```

You should see output similar to:
```
GStreamer:                   YES (1.16.2)
```

## Build and Run

### 1. Build Project

```bash
# Enter project directory
cd camera-gstreamer

# Execute build script
chmod +x build.sh
./build.sh
```

### 2. Clean Build

```bash
./build.sh clean
```

### 3. Run Program

#### Run with default parameters
```bash
./build/camera_gstreamer
```

Default parameters:
- Device: `/dev/video0`
- Resolution: 1920x1080
- Frame rate: 30 FPS
- Queue size: 5

#### Run with custom parameters
```bash
./build/camera_gstreamer [device] [width] [height] [fps] [queue_size]
```

Examples:
```bash
# Use /dev/video0, resolution 640x480, frame rate 30, queue size 5
./build/camera_gstreamer /dev/video0 640 480 30 5

# Use /dev/video1, resolution 1920x1080, frame rate 60
./build/camera_gstreamer /dev/video1 1920 1080 60
```

#### Exit Program
- Press `q` key
- Press `ESC` key
- Press `Ctrl+C`

## V4L2 Camera Debugging

### List all camera devices
```bash
v4l2-ctl --list-devices
```

### View device supported formats and resolutions
```bash
v4l2-ctl --device=/dev/video0 --list-formats-ext
```

### View device current settings
```bash
v4l2-ctl --device=/dev/video0 --all
```

### Test GStreamer Pipeline
```bash
# Test basic v4l2src pipeline
gst-launch-1.0 v4l2src device=/dev/video0 ! videoconvert ! autovideosink

# Test with specified resolution and frame rate
gst-launch-1.0 v4l2src device=/dev/video0 ! \
  video/x-raw,width=1920,height=1080,framerate=30/1 ! \
  videoconvert ! autovideosink
```

## Program Architecture

### Threading Model

```
┌─────────────────┐         ┌──────────────┐         ┌─────────────────┐
│  Capture Thread │  push   │  Frame Queue │  pop    │  Display Thread │
│  (Capture)      │ ───────>│ (FrameQueue) │ ───────>│  (Display)      │
│                 │         │              │         │                 │
│ - V4L2 Capture  │         │ - Thread-safe│         │ - OpenCV Display│
│ - GStreamer     │         │ - Max queue 5│         │ - FPS Stats     │
│ - FPS Stats     │         │ - Auto drop  │         │ - Key Detection │
└─────────────────┘         └──────────────┘         └─────────────────┘
```

### Key Components

1. **FrameQueue**: Thread-safe frame queue
   - Implemented using `std::mutex` and `std::condition_variable`
   - Automatically drops old frames to prevent queue overflow
   - Supports timeout waiting

2. **captureThread**: Capture thread
   - Opens V4L2 device using GStreamer backend
   - Continuously reads frames and pushes to queue
   - Tracks capture FPS

3. **displayThread**: Display thread
   - Retrieves frames from queue
   - Displays using OpenCV
   - Tracks display FPS
   - Handles user input

## GStreamer Pipeline Description

Default pipeline used in the program:

```
v4l2src device=/dev/video0 ! 
  video/x-raw,width=1920,height=1080,framerate=30/1 ! 
  videoconvert ! 
  appsink
```

Component descriptions:
- `v4l2src`: V4L2 video source
- `video/x-raw`: Specifies raw video format and parameters
- `videoconvert`: Format conversion (converts to BGR for OpenCV)
- `appsink`: Application sink (OpenCV receives data)

## Performance Optimization Tips

1. **Adjust queue size**: Adjust `queue_size` parameter based on system performance
2. **Reduce resolution**: Try lower resolution if FPS is unstable
3. **Use hardware acceleration**: Rockchip platforms can use MPP hardware decoding
4. **Optimize GStreamer Pipeline**: Use hardware codec plugins

## Common Issues

### 1. Camera cannot be opened
- Check if device path is correct: `ls -l /dev/video*`
- Check permissions: `sudo chmod 666 /dev/video0`
- Check if device is in use: `sudo fuser /dev/video0`

### 2. GStreamer errors
- Check if all GStreamer plugins are installed
- Verify OpenCV has GStreamer support
- Try testing pipeline with `gst-launch-1.0`

### 3. Compilation errors
- Ensure all dependency libraries are installed
- Check OpenCV and GStreamer version compatibility
- Review CMake configuration output

### 4. Low FPS
- Reduce resolution
- Increase queue size
- Check CPU usage
- Try hardware acceleration

## License

This project is example code and may be freely used and modified.

## Author

GitHub Copilot Assistant

## References

- [OpenCV Documentation](https://docs.opencv.org/)
- [GStreamer Documentation](https://gstreamer.freedesktop.org/documentation/)
- [V4L2 Documentation](https://www.kernel.org/doc/html/latest/userspace-api/media/v4l/v4l2.html)
