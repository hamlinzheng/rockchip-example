#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <chrono>

// Build GStreamer pipeline string for V4L2 device
std::string buildGstreamerPipeline(const std::string& device, int width, int height, int fps) {
    std::string pipeline = "v4l2src device=" + device + " min-buffers=2 io-mode=mmap ! ";
    pipeline += "video/x-raw, width=(int)" + std::to_string(width) + ", height=(int)" + std::to_string(height) + ", framerate=(fraction)" + std::to_string(fps) + "/1 ! ";
    pipeline += "videoconvert ! video/x-raw, format=(string)BGR ! appsink";
    return pipeline;
}

int main(int argc, char** argv) {
    // Check command line arguments
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <v4l2_device> [width] [height] [fps]" << std::endl;
        return -1;
    }

    std::string device = argv[1];
    int width = (argc >= 3) ? std::stoi(argv[2]) : 1280;
    int height = (argc >= 4) ? std::stoi(argv[3]) : 720;
    int fps = (argc >= 5) ? std::stoi(argv[4]) : 30;

    // Build GStreamer pipeline
    std::string pipeline = buildGstreamerPipeline(device, width, height, fps);
    std::cout << "Using GStreamer pipeline: " << pipeline << std::endl;

    // Open video stream
    cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);
    if (!cap.isOpened()) {
        std::cerr << "Failed to open video stream" << std::endl;
        return -1;
    }

    // Get actual video size
    width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    std::cout << "Video size: " << width << "x" << height << std::endl;

    // Create window
    cv::namedWindow("GStreamer Video", cv::WINDOW_NORMAL);
    cv::resizeWindow("GStreamer Video", width, height);

    cv::Mat frame;
    auto prevTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;

    std::cout << "Start playing video..." << std::endl;
    std::cout << "Press 'q' to quit" << std::endl;

    while (true) {
        // Read a frame
        cap >> frame;

        // Check if frame is valid
        if (frame.empty()) {
            std::cerr << "Failed to read frame" << std::endl;
            break;
        }

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
    cap.release();
    cv::destroyAllWindows();

    std::cout << std::endl << "Exited." << std::endl;

    return 0;
}