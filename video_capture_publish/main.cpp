#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include <cstdlib>

// Build GStreamer pipeline string for V4L2 device
std::string buildGstreamerPipeline(const std::string& device, int width, int height, int fps) {
    std::string pipeline = "v4l2src device=" + device + " min-buffers=2 io-mode=mmap ! ";
    pipeline += "video/x-raw, width=(int)" + std::to_string(width) + ", height=(int)" + std::to_string(height) + ", framerate=(fraction)" + std::to_string(fps) + "/1 ! ";
    pipeline += "videoconvert ! video/x-raw, format=(string)BGR ! appsink";
    return pipeline;
}

// Build GStreamer pipeline string for RTSP streaming
std::string buildRtspOutputPipeline(const std::string& rtspUrl, int width, int height, int fps) {
    std::string pipeline = "appsrc ! videoconvert ! mpph264enc ! h264parse ! rtspclientsink location=";
    pipeline += rtspUrl;
    return pipeline;
}

int main(int argc, char** argv) {
    // Set environment variables for RGA hardware acceleration
    setenv("GST_VIDEO_CONVERT_USE_RGA", "1", 1);
    setenv("GST_VIDEO_FLIP_USE_RGA", "1", 1);

    // Check command line arguments
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <v4l2_device> <rtsp_url> [width] [height] [fps]" << std::endl;
        return -1;
    }

    std::string device = argv[1];
    std::string rtspUrl = argv[2];
    int width = (argc >= 4) ? std::stoi(argv[3]) : 1280;
    int height = (argc >= 5) ? std::stoi(argv[4]) : 720;
    int fps = (argc >= 6) ? std::stoi(argv[5]) : 30;

    // Build input and output GStreamer pipelines
    std::string inputPipeline = buildGstreamerPipeline(device, width, height, fps);
    std::string outputPipeline = buildRtspOutputPipeline(rtspUrl, width, height, fps);
    
    std::cout << "Input pipeline: " << inputPipeline << std::endl;
    std::cout << "Output pipeline: " << outputPipeline << std::endl;

    // Open video stream
    cv::VideoCapture cap(inputPipeline, cv::CAP_GSTREAMER);
    if (!cap.isOpened()) {
        std::cerr << "Failed to open video stream" << std::endl;
        return -1;
    }

    // Get actual video size
    width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    std::cout << "Video size: " << width << "x" << height << std::endl;

    // Create video writer for RTSP streaming
    cv::VideoWriter writer(outputPipeline, 0, (double)fps, cv::Size(width, height), true);
    if (!writer.isOpened()) {
        std::cerr << "Failed to open RTSP output stream" << std::endl;
        return -1;
    }

    // Create window
    cv::namedWindow("GStreamer Video", cv::WINDOW_NORMAL);
    cv::resizeWindow("GStreamer Video", width, height);

    cv::Mat frame;
    auto prevTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;

    std::cout << "Start playing video and streaming to RTSP..." << std::endl;
    std::cout << "Press 'q' to quit" << std::endl;

    while (true) {
        // Read a frame
        cap >> frame;

        // Check if frame is valid
        if (frame.empty()) {
            std::cerr << "Failed to read frame" << std::endl;
            break;
        }

        // Write frame to RTSP stream
        writer.write(frame);

        // Calculate FPS
        frameCount++;
        auto currTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(currTime - prevTime).count();

        if (duration >= 1) {
            std::cout << "\rCurrent FPS: " << frameCount / duration << "    " << std::flush;
            frameCount = 0;
            prevTime = currTime;
        }

        // Show frame
        cv::imshow("GStreamer Video", frame);

        // Press 'q' to quit
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    // Release resources
    writer.release();
    cap.release();
    cv::destroyAllWindows();

    std::cout << std::endl << "Exited." << std::endl;

    return 0;
}