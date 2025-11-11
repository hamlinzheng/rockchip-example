#include <opencv2/opencv.hpp>
#include <opencv2/core/ocl.hpp>
#include <iostream>

int main(int argc, char** argv) {
    std::cout << "=== OpenCV OpenCL Support Check ===" << std::endl;
    std::cout << "OpenCV Version: " << CV_VERSION << std::endl;
    std::cout << std::endl;

    // Check if OpenCL is available
    bool hasOpenCL = cv::ocl::haveOpenCL();
    std::cout << "OpenCL Available: " << (hasOpenCL ? "YES" : "NO") << std::endl;

    if (hasOpenCL) {
        // Check if OpenCL is enabled
        bool useOpenCL = cv::ocl::useOpenCL();
        std::cout << "OpenCL Enabled: " << (useOpenCL ? "YES" : "NO") << std::endl;
        
        // Get OpenCL device information
        cv::ocl::Device device = cv::ocl::Device::getDefault();
        if (device.available()) {
            std::cout << std::endl;
            std::cout << "=== OpenCL Device Information ===" << std::endl;
            std::cout << "Device Name: " << device.name() << std::endl;
            std::cout << "Device Vendor: " << device.vendorName() << std::endl;
            std::cout << "Device Version: " << device.version() << std::endl;
            std::cout << "Driver Version: " << device.driverVersion() << std::endl;
            std::cout << "OpenCL Version: " << device.OpenCL_C_Version() << std::endl;
            std::cout << "Compute Units: " << device.maxComputeUnits() << std::endl;
            std::cout << "Max Work Group Size: " << device.maxWorkGroupSize() << std::endl;
            std::cout << "Global Memory Size: " << (device.globalMemSize() / 1024 / 1024) << " MB" << std::endl;
            std::cout << "Local Memory Size: " << (device.localMemSize() / 1024) << " KB" << std::endl;
            std::cout << "Device Type: ";
            if (device.type() == cv::ocl::Device::TYPE_CPU) {
                std::cout << "CPU" << std::endl;
            } else if (device.type() == cv::ocl::Device::TYPE_GPU) {
                std::cout << "GPU" << std::endl;
            } else if (device.type() == cv::ocl::Device::TYPE_ACCELERATOR) {
                std::cout << "ACCELERATOR" << std::endl;
            } else {
                std::cout << "OTHER" << std::endl;
            }
        }

        // List all available platforms and devices
        std::cout << std::endl;
        std::cout << "=== Available Platforms and Devices ===" << std::endl;
        std::vector<cv::ocl::PlatformInfo> platforms;
        cv::ocl::getPlatfomsInfo(platforms);
        
        for (size_t i = 0; i < platforms.size(); i++) {
            std::cout << "\nPlatform " << i << ":" << std::endl;
            std::cout << "  Name: " << platforms[i].name() << std::endl;
            std::cout << "  Vendor: " << platforms[i].vendor() << std::endl;
            std::cout << "  Version: " << platforms[i].version() << std::endl;
            
            int deviceCount = platforms[i].deviceNumber();
            std::cout << "  Device Count: " << deviceCount << std::endl;
            
            for (int j = 0; j < deviceCount; j++) {
                cv::ocl::Device dev;
                platforms[i].getDevice(dev, j);
                std::cout << "    Device " << j << ": " << dev.name() << std::endl;
            }
        }

        // Try to enable OpenCL if not already enabled
        if (!useOpenCL) {
            std::cout << std::endl;
            std::cout << "Attempting to enable OpenCL..." << std::endl;
            cv::ocl::setUseOpenCL(true);
            std::cout << "OpenCL Enabled: " << (cv::ocl::useOpenCL() ? "YES" : "NO") << std::endl;
        }

        // Perform a simple benchmark to test OpenCL performance
        std::cout << std::endl;
        std::cout << "=== Performance Benchmark ===" << std::endl;
        
        cv::Mat cpuMat = cv::Mat::ones(2048, 2048, CV_32FC1);
        cv::UMat gpuMat = cpuMat.getUMat(cv::ACCESS_READ);
        
        // Benchmark Gaussian Blur
        cv::Mat resultCpu;
        cv::UMat resultGpu;
        
        double cpuTime, gpuTime;
        int iterations = 10;
        
        // CPU benchmark
        cv::ocl::setUseOpenCL(false);
        double t1 = (double)cv::getTickCount();
        for (int i = 0; i < iterations; i++) {
            cv::GaussianBlur(cpuMat, resultCpu, cv::Size(15, 15), 3.0);
        }
        cpuTime = ((double)cv::getTickCount() - t1) / cv::getTickFrequency() * 1000 / iterations;
        
        // GPU benchmark
        cv::ocl::setUseOpenCL(true);
        t1 = (double)cv::getTickCount();
        for (int i = 0; i < iterations; i++) {
            cv::GaussianBlur(gpuMat, resultGpu, cv::Size(15, 15), 3.0);
        }
        gpuTime = ((double)cv::getTickCount() - t1) / cv::getTickFrequency() * 1000 / iterations;
        
        std::cout << "GaussianBlur (2048x2048, kernel 15x15):" << std::endl;
        std::cout << "  CPU Time: " << cpuTime << " ms" << std::endl;
        std::cout << "  GPU Time: " << gpuTime << " ms" << std::endl;
        std::cout << "  Speedup: " << (cpuTime / gpuTime) << "x" << std::endl;
        
    } else {
        std::cout << std::endl;
        std::cout << "OpenCL is not available in this OpenCV build." << std::endl;
        std::cout << "Please rebuild OpenCV with OpenCL support enabled." << std::endl;
    }

    return 0;
}
