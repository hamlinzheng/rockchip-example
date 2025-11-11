/**
 * OpenCL Native Vector Computation Example
 * 
 * This program demonstrates basic OpenCL usage for vector operations:
 * - Platform and device detection
 * - Context and command queue creation
 * - Kernel compilation and execution
 * - Memory buffer management
 * - Vector addition: C = A + B
 */

#include <CL/cl.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>

// OpenCL kernel source code for vector addition
const char* kernelSource = R"(
__kernel void vector_add(__global const float* A,
                         __global const float* B,
                         __global float* C,
                         const unsigned int n)
{
    // Get global thread ID
    int id = get_global_id(0);
    
    // Perform vector addition if within bounds
    if (id < n) {
        C[id] = A[id] + B[id];
    }
}
)";

// Helper function to check OpenCL errors
void checkError(cl_int error, const char* operation) {
    if (error != CL_SUCCESS) {
        std::cerr << "Error during operation '" << operation 
                  << "': " << error << std::endl;
        exit(EXIT_FAILURE);
    }
}

// Function to print device information
void printDeviceInfo(cl_device_id device) {
    char deviceName[256];
    char deviceVendor[256];
    cl_uint computeUnits;
    size_t workGroupSize;
    cl_ulong globalMemSize;
    cl_ulong localMemSize;
    
    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(deviceName), deviceName, nullptr);
    clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(deviceVendor), deviceVendor, nullptr);
    clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(computeUnits), &computeUnits, nullptr);
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(workGroupSize), &workGroupSize, nullptr);
    clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(globalMemSize), &globalMemSize, nullptr);
    clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(localMemSize), &localMemSize, nullptr);
    
    std::cout << "\n=== Device Information ===" << std::endl;
    std::cout << "Device Name: " << deviceName << std::endl;
    std::cout << "Vendor: " << deviceVendor << std::endl;
    std::cout << "Compute Units: " << computeUnits << std::endl;
    std::cout << "Max Work Group Size: " << workGroupSize << std::endl;
    std::cout << "Global Memory: " << (globalMemSize / (1024 * 1024)) << " MB" << std::endl;
    std::cout << "Local Memory: " << (localMemSize / 1024) << " KB" << std::endl;
    std::cout << "========================\n" << std::endl;
}

int main(int argc, char** argv) {
    // Vector size
    const unsigned int VECTOR_SIZE = 1024;
    const size_t dataSize = sizeof(float) * VECTOR_SIZE;
    
    // Initialize input vectors
    std::vector<float> h_A(VECTOR_SIZE);
    std::vector<float> h_B(VECTOR_SIZE);
    std::vector<float> h_C(VECTOR_SIZE);
    
    // Fill input vectors with sample data
    for (unsigned int i = 0; i < VECTOR_SIZE; i++) {
        h_A[i] = static_cast<float>(i);
        h_B[i] = static_cast<float>(i * 2);
    }
    
    std::cout << "OpenCL Vector Addition Example" << std::endl;
    std::cout << "Vector Size: " << VECTOR_SIZE << std::endl;
    
    // Step 1: Get platform information
    cl_uint numPlatforms;
    cl_int error = clGetPlatformIDs(0, nullptr, &numPlatforms);
    checkError(error, "Getting platform count");
    
    if (numPlatforms == 0) {
        std::cerr << "No OpenCL platforms found!" << std::endl;
        return EXIT_FAILURE;
    }
    
    std::cout << "Found " << numPlatforms << " OpenCL platform(s)" << std::endl;
    
    std::vector<cl_platform_id> platforms(numPlatforms);
    error = clGetPlatformIDs(numPlatforms, platforms.data(), nullptr);
    checkError(error, "Getting platform IDs");
    
    // Get platform name
    char platformName[256];
    clGetPlatformInfo(platforms[0], CL_PLATFORM_NAME, sizeof(platformName), platformName, nullptr);
    std::cout << "Using platform: " << platformName << std::endl;
    
    // Step 2: Get device information
    cl_uint numDevices;
    error = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 0, nullptr, &numDevices);
    
    // If no GPU found, try CPU
    if (error != CL_SUCCESS || numDevices == 0) {
        std::cout << "No GPU found, trying CPU..." << std::endl;
        error = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_CPU, 0, nullptr, &numDevices);
        checkError(error, "Getting CPU device count");
    }
    
    if (numDevices == 0) {
        std::cerr << "No OpenCL devices found!" << std::endl;
        return EXIT_FAILURE;
    }
    
    std::vector<cl_device_id> devices(numDevices);
    error = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, numDevices, devices.data(), nullptr);
    checkError(error, "Getting device IDs");
    
    // Use the first available device
    cl_device_id device = devices[0];
    printDeviceInfo(device);
    
    // Step 3: Create OpenCL context
    cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &error);
    checkError(error, "Creating context");
    
    // Step 4: Create command queue
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, &error);
    checkError(error, "Creating command queue");
    
    // Step 5: Create memory buffers on the device
    cl_mem d_A = clCreateBuffer(context, CL_MEM_READ_ONLY, dataSize, nullptr, &error);
    checkError(error, "Creating buffer A");
    
    cl_mem d_B = clCreateBuffer(context, CL_MEM_READ_ONLY, dataSize, nullptr, &error);
    checkError(error, "Creating buffer B");
    
    cl_mem d_C = clCreateBuffer(context, CL_MEM_WRITE_ONLY, dataSize, nullptr, &error);
    checkError(error, "Creating buffer C");
    
    // Step 6: Copy input data to device
    error = clEnqueueWriteBuffer(queue, d_A, CL_TRUE, 0, dataSize, h_A.data(), 0, nullptr, nullptr);
    checkError(error, "Writing buffer A");
    
    error = clEnqueueWriteBuffer(queue, d_B, CL_TRUE, 0, dataSize, h_B.data(), 0, nullptr, nullptr);
    checkError(error, "Writing buffer B");
    
    // Step 7: Create and compile the OpenCL program
    cl_program program = clCreateProgramWithSource(context, 1, &kernelSource, nullptr, &error);
    checkError(error, "Creating program");
    
    error = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    if (error != CL_SUCCESS) {
        // Print build log if compilation fails
        size_t logSize;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
        std::vector<char> log(logSize);
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize, log.data(), nullptr);
        std::cerr << "Build error:\n" << log.data() << std::endl;
        checkError(error, "Building program");
    }
    
    std::cout << "OpenCL kernel compiled successfully" << std::endl;
    
    // Step 8: Create the kernel
    cl_kernel kernel = clCreateKernel(program, "vector_add", &error);
    checkError(error, "Creating kernel");
    
    // Step 9: Set kernel arguments
    error = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_A);
    checkError(error, "Setting kernel arg 0");
    
    error = clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_B);
    checkError(error, "Setting kernel arg 1");
    
    error = clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_C);
    checkError(error, "Setting kernel arg 2");
    
    error = clSetKernelArg(kernel, 3, sizeof(unsigned int), &VECTOR_SIZE);
    checkError(error, "Setting kernel arg 3");
    
    // Step 10: Execute the kernel
    size_t globalWorkSize = VECTOR_SIZE;
    size_t localWorkSize = 64; // Work-group size
    
    std::cout << "Executing kernel..." << std::endl;
    error = clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &globalWorkSize, &localWorkSize, 0, nullptr, nullptr);
    checkError(error, "Executing kernel");
    
    // Step 11: Wait for kernel execution to complete
    error = clFinish(queue);
    checkError(error, "Waiting for kernel completion");
    
    std::cout << "Kernel execution completed" << std::endl;
    
    // Step 12: Read the result back to host memory
    error = clEnqueueReadBuffer(queue, d_C, CL_TRUE, 0, dataSize, h_C.data(), 0, nullptr, nullptr);
    checkError(error, "Reading result buffer");
    
    // Step 13: Verify results
    std::cout << "\nVerifying results..." << std::endl;
    bool success = true;
    int errorsFound = 0;
    const int maxErrorsToPrint = 5;
    
    for (unsigned int i = 0; i < VECTOR_SIZE; i++) {
        float expected = h_A[i] + h_B[i];
        if (std::abs(h_C[i] - expected) > 1e-5) {
            if (errorsFound < maxErrorsToPrint) {
                std::cerr << "Mismatch at index " << i 
                          << ": Expected " << expected 
                          << ", Got " << h_C[i] << std::endl;
            }
            errorsFound++;
            success = false;
        }
    }
    
    if (success) {
        std::cout << "✓ All results are correct!" << std::endl;
        std::cout << "\nSample results (first 10 elements):" << std::endl;
        for (int i = 0; i < 10; i++) {
            std::cout << "  " << h_A[i] << " + " << h_B[i] 
                      << " = " << h_C[i] << std::endl;
        }
    } else {
        std::cout << "✗ Found " << errorsFound << " errors!" << std::endl;
    }
    
    // Step 14: Cleanup resources
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseMemObject(d_A);
    clReleaseMemObject(d_B);
    clReleaseMemObject(d_C);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    
    std::cout << "\nOpenCL resources cleaned up successfully" << std::endl;
    
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}