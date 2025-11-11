#include <opencv2/opencv.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>
#include <chrono>

// Function to load camera intrinsic parameters from YAML file
bool loadCameraParams(const std::string& filename, 
                      cv::Mat& cameraMatrix, 
                      cv::Mat& distCoeffs,
                      cv::Size& imageSize) {
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }

    // Read camera matrix (K)
    fs["camera_matrix"] >> cameraMatrix;
    
    // Read distortion coefficients
    fs["dist_coeffs"] >> distCoeffs;
    
    // Read image resolution
    cv::Mat resolution;
    fs["resolution"] >> resolution;
    imageSize = cv::Size(resolution.at<int>(0), resolution.at<int>(1));
    
    fs.release();
    
    // Validate parameters
    if (cameraMatrix.empty() || distCoeffs.empty()) {
        std::cerr << "Error: Invalid camera parameters" << std::endl;
        return false;
    }
    
    return true;
}

// Function to perform fisheye undistortion with timing
void undistortFisheye(const cv::Mat& src, cv::Mat& dst,
                      const cv::Mat& cameraMatrix,
                      const cv::Mat& distCoeffs,
                      bool useOpenCL,
                      double& elapsedTime) {
    // Set OpenCL usage
    cv::ocl::setUseOpenCL(useOpenCL);
    
    // Compute new camera matrix for undistortion
    cv::Mat newCameraMatrix = cameraMatrix.clone();
    
    // Compute undistortion maps
    cv::Mat map1, map2;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    if (useOpenCL) {
        // Use UMat for GPU acceleration
        cv::UMat uSrc = src.getUMat(cv::ACCESS_READ);
        cv::UMat uDst;
        cv::UMat uMap1, uMap2;
        
        // Initialize fisheye undistortion maps
        cv::fisheye::initUndistortRectifyMap(
            cameraMatrix, 
            distCoeffs, 
            cv::Mat(),
            newCameraMatrix,
            src.size(),
            CV_16SC2,
            map1,
            map2
        );
        
        // Convert maps to UMat for GPU processing
        uMap1 = map1.getUMat(cv::ACCESS_READ);
        uMap2 = map2.getUMat(cv::ACCESS_READ);
        
        // Perform remap operation on GPU
        cv::remap(uSrc, uDst, uMap1, uMap2, cv::INTER_LINEAR);
        
        // Download result from GPU
        uDst.copyTo(dst);
    } else {
        // CPU-only processing
        cv::fisheye::initUndistortRectifyMap(
            cameraMatrix, 
            distCoeffs, 
            cv::Mat(),
            newCameraMatrix,
            src.size(),
            CV_16SC2,
            map1,
            map2
        );
        
        cv::remap(src, dst, map1, map2, cv::INTER_LINEAR);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    elapsedTime = std::chrono::duration<double, std::milli>(end - start).count();
}

// Function to display OpenCL information
void displayOpenCLInfo() {
    std::cout << "=== OpenCL Information ===" << std::endl;
    std::cout << "OpenCV Version: " << CV_VERSION << std::endl;
    
    bool hasOpenCL = cv::ocl::haveOpenCL();
    std::cout << "OpenCL Available: " << (hasOpenCL ? "YES" : "NO") << std::endl;
    
    if (hasOpenCL) {
        cv::ocl::Device device = cv::ocl::Device::getDefault();
        if (device.available()) {
            std::cout << "Device Name: " << device.name() << std::endl;
            std::cout << "Device Vendor: " << device.vendorName() << std::endl;
            std::cout << "Compute Units: " << device.maxComputeUnits() << std::endl;
        }
    }
    std::cout << std::endl;
}

int main(int argc, char** argv) {
    std::cout << "=== Fisheye Image Undistortion with OpenCV + OpenCL ===" << std::endl;
    std::cout << std::endl;
    
    // Display OpenCL information
    displayOpenCLInfo();
    
    // File paths
    std::string imagePath = "image.png";
    std::string intrinsicPath = "intrinsic.yaml";
    
    // Allow custom paths via command line arguments
    if (argc > 1) {
        imagePath = argv[1];
    }
    if (argc > 2) {
        intrinsicPath = argv[2];
    }
    
    // Load input image
    std::cout << "Loading image: " << imagePath << std::endl;
    cv::Mat srcImage = cv::imread(imagePath);
    if (srcImage.empty()) {
        std::cerr << "Error: Cannot load image " << imagePath << std::endl;
        return -1;
    }
    std::cout << "Image size: " << srcImage.cols << "x" << srcImage.rows << std::endl;
    std::cout << std::endl;
    
    // Load camera parameters
    std::cout << "Loading camera parameters: " << intrinsicPath << std::endl;
    cv::Mat cameraMatrix, distCoeffs;
    cv::Size imageSize;
    if (!loadCameraParams(intrinsicPath, cameraMatrix, distCoeffs, imageSize)) {
        return -1;
    }
    
    std::cout << "Camera Matrix:" << std::endl << cameraMatrix << std::endl;
    std::cout << "Distortion Coefficients:" << std::endl << distCoeffs.t() << std::endl;
    std::cout << "Expected image size: " << imageSize.width << "x" << imageSize.height << std::endl;
    std::cout << std::endl;
    
    // Resize image if necessary to match calibration size
    if (srcImage.size() != imageSize) {
        std::cout << "Warning: Image size differs from calibration. Resizing..." << std::endl;
        cv::resize(srcImage, srcImage, imageSize);
    }
    
    // Perform undistortion with CPU
    std::cout << "=== Processing ===" << std::endl;
    cv::Mat dstCPU;
    double cpuTime;
    std::cout << "Undistorting with CPU..." << std::endl;
    undistortFisheye(srcImage, dstCPU, cameraMatrix, distCoeffs, false, cpuTime);
    std::cout << "CPU processing time: " << cpuTime << " ms" << std::endl;
    
    // Perform undistortion with GPU (OpenCL) if available
    cv::Mat dstGPU;
    double gpuTime = 0.0;
    if (cv::ocl::haveOpenCL()) {
        std::cout << "Undistorting with GPU (OpenCL)..." << std::endl;
        undistortFisheye(srcImage, dstGPU, cameraMatrix, distCoeffs, true, gpuTime);
        std::cout << "GPU processing time: " << gpuTime << " ms" << std::endl;
        std::cout << "Speedup: " << (cpuTime / gpuTime) << "x" << std::endl;
    } else {
        std::cout << "OpenCL not available, skipping GPU processing" << std::endl;
    }
    std::cout << std::endl;
    
    // Save results
    std::string outputCPU = "undistorted_cpu.png";
    std::string outputGPU = "undistorted_gpu.png";
    
    cv::imwrite(outputCPU, dstCPU);
    std::cout << "Saved CPU result to: " << outputCPU << std::endl;
    
    if (!dstGPU.empty()) {
        cv::imwrite(outputGPU, dstGPU);
        std::cout << "Saved GPU result to: " << outputGPU << std::endl;
    }
    
    // Display images (comment out if running headless)
    std::cout << std::endl;
    std::cout << "Displaying images... (Press any key to exit)" << std::endl;
    
    // Create comparison image
    cv::Mat comparison;
    if (!dstGPU.empty()) {
        // Show original, CPU result, and GPU result side by side
        cv::Mat top, bottom;
        
        // Resize for display if images are too large
        int maxWidth = 1920 / 3;
        double scale = (double)maxWidth / srcImage.cols;
        if (scale > 1.0) scale = 1.0;
        
        cv::Mat displaySrc, displayCPU, displayGPU;
        cv::resize(srcImage, displaySrc, cv::Size(), scale, scale);
        cv::resize(dstCPU, displayCPU, cv::Size(), scale, scale);
        cv::resize(dstGPU, displayGPU, cv::Size(), scale, scale);
        
        // Add labels
        cv::putText(displaySrc, "Original (Fisheye)", cv::Point(10, 30),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
        cv::putText(displayCPU, "CPU Undistorted", cv::Point(10, 30),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
        cv::putText(displayGPU, "GPU Undistorted", cv::Point(10, 30),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
        
        cv::hconcat(displaySrc, displayCPU, top);
        cv::hconcat(top, displayGPU, comparison);
    } else {
        // Show only original and CPU result
        cv::Mat displaySrc, displayCPU;
        double scale = 960.0 / srcImage.cols;
        if (scale > 1.0) scale = 1.0;
        
        cv::resize(srcImage, displaySrc, cv::Size(), scale, scale);
        cv::resize(dstCPU, displayCPU, cv::Size(), scale, scale);
        
        cv::putText(displaySrc, "Original (Fisheye)", cv::Point(10, 30),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
        cv::putText(displayCPU, "CPU Undistorted", cv::Point(10, 30),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
        
        cv::hconcat(displaySrc, displayCPU, comparison);
    }
    
    cv::imshow("Fisheye Undistortion Comparison", comparison);
    cv::waitKey(0);
    
    std::cout << "Done!" << std::endl;
    return 0;
}
