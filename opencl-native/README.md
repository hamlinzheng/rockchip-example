# OpenCL Native Vector Computation Example

This is a native OpenCL example that demonstrates basic vector computation (vector addition) on GPU or CPU devices.

## Features

- **Platform and Device Detection**: Automatically detects available OpenCL platforms and devices
- **Vector Addition**: Performs parallel vector addition: C = A + B
- **Memory Management**: Demonstrates proper buffer creation and data transfer
- **Error Handling**: Comprehensive error checking for all OpenCL operations
- **Device Information**: Displays detailed device capabilities
- **Result Verification**: Validates computed results against expected values

## Prerequisites

- C++ compiler (g++, clang++)
- CMake 3.10 or higher
- OpenCL SDK (headers and libraries)

### Installing OpenCL on Linux

```bash
# For Ubuntu/Debian
sudo apt-get install opencl-headers ocl-icd-opencl-dev

# For GPU support (NVIDIA)
sudo apt-get install nvidia-opencl-dev

# For GPU support (AMD)
sudo apt-get install mesa-opencl-icd

# For CPU support (Intel)
sudo apt-get install intel-opencl-icd
```

## Building

### Using the build script (recommended):

```bash
./build.sh
```

### Manual build:

```bash
mkdir -p build
cd build
cmake ..
make
```

## Running

After building, run the executable:

```bash
./build/opencl_native
```

## Expected Output

The program will:
1. Display available OpenCL platforms and devices
2. Show device information (compute units, memory, etc.)
3. Compile and execute the vector addition kernel
4. Verify the results
5. Display sample outputs

## Code Structure

- **main.cpp**: Complete OpenCL implementation with detailed comments
  - Platform/device enumeration
  - Context and command queue creation
  - Kernel compilation and execution
  - Memory buffer management
  - Result verification
  
- **CMakeLists.txt**: Build configuration
  - Finds OpenCL package
  - Sets up include paths and linking
  - Configures compiler flags

- **build.sh**: Automated build script

## OpenCL Kernel

The kernel performs simple vector addition:

```c
C[id] = A[id] + B[id]
```

Each work-item processes one element of the vectors in parallel.

## Customization

You can modify the following parameters in `main.cpp`:

- `VECTOR_SIZE`: Size of the vectors (default: 1024)
- `localWorkSize`: Work-group size (default: 64)
- Input data initialization logic

## Troubleshooting

**No OpenCL platforms found:**
- Ensure OpenCL drivers are installed for your hardware
- Check if `clinfo` command works

**Build errors:**
- Verify OpenCL SDK is installed
- Check CMake finds OpenCL correctly

**Runtime errors:**
- Check device compatibility
- Ensure sufficient memory is available
- Review kernel build log if compilation fails

## License

This is an educational example for learning OpenCL programming.
