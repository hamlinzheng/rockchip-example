# OpenCV OpenCL Examples

This directory contains examples demonstrating OpenCV with OpenCL acceleration.

## Programs

### 1. check_opencv - OpenCV OpenCL Support Check

This program checks if OpenCV has OpenCL support enabled and provides detailed information about available OpenCL devices and performance.

**Features:**
- Checks if OpenCL is available in the OpenCV build
- Displays OpenCL device information (name, vendor, version, memory, etc.)
- Lists all available OpenCL platforms and devices
- Performs a simple performance benchmark comparing CPU vs GPU execution
- Shows speedup when using OpenCL acceleration

### 2. fisheye_undistortion - Fisheye Image Undistortion

This program performs fisheye lens distortion correction using OpenCV's fisheye calibration model with OpenCL acceleration.

**Features:**
- Loads camera intrinsic parameters from YAML file
- Performs fisheye undistortion on both CPU and GPU
- Compares performance between CPU and GPU processing
- Displays side-by-side comparison of original and undistorted images
- Saves undistorted results to files

## Building

```bash
./build.sh
```

Or manually:

```bash
mkdir -p build
cd build
cmake ..
make
```

## Running

### Check OpenCV OpenCL Support
```bash
./build/check_opencv
```

### Fisheye Undistortion
```bash
cd build
./fisheye_undistortion
# Or specify custom image and intrinsic file paths
./fisheye_undistortion <image_path> <intrinsic_yaml_path>
```

**Required Files:**
- `image.png` - Input fisheye image
- `intrinsic.yaml` - Camera intrinsic parameters (camera matrix and distortion coefficients)

**Output Files:**
- `undistorted_cpu.png` - Result from CPU processing
- `undistorted_gpu.png` - Result from GPU processing (if OpenCL available)

## Requirements

- OpenCV with OpenCL support
- CMake 3.10+
- C++11 compatible compiler
- OpenCL drivers installed on the system

## Sample Output

The program will display:
- OpenCV version
- OpenCL availability status
- Device information (if available)
- List of all platforms and devices
- Performance benchmark results (CPU vs GPU)

## Notes

If OpenCL is not available, the program will indicate that OpenCV was built without OpenCL support. You would need to rebuild OpenCV with `-DWITH_OPENCL=ON` flag to enable it.
