#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <chrono>

// Thread-safe frame queue
class FrameQueue {
private:
    std::queue<cv::Mat> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
    size_t max_size_;
    std::atomic<bool> running_;

public:
    FrameQueue(size_t max_size = 5) : max_size_(max_size), running_(true) {}

    void push(const cv::Mat& frame) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        // If queue is full, remove the oldest frame
        while (queue_.size() >= max_size_ && running_) {
            queue_.pop();
        }
        
        if (running_) {
            queue_.push(frame.clone());
            cond_.notify_one();
        }
    }

    bool pop(cv::Mat& frame, int timeout_ms = 1000) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        // Wait for frame available or timeout
        if (!cond_.wait_for(lock, std::chrono::milliseconds(timeout_ms), 
                            [this] { return !queue_.empty() || !running_; })) {
            return false;
        }

        if (!running_ && queue_.empty()) {
            return false;
        }

        if (!queue_.empty()) {
            frame = queue_.front();
            queue_.pop();
            return true;
        }

        return false;
    }

    void stop() {
        running_ = false;
        cond_.notify_all();
    }

    bool is_running() const {
        return running_;
    }

    size_t size() {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }
};

// Capture thread function
void captureThread(FrameQueue& frameQueue, const std::string& gst_pipeline, 
                   std::atomic<bool>& running, std::atomic<int>& capture_fps) {
    std::cout << "Capture thread starting..." << std::endl;
    std::cout << "GStreamer Pipeline: " << gst_pipeline << std::endl;

    // Open camera using GStreamer
    cv::VideoCapture cap(gst_pipeline, cv::CAP_GSTREAMER);
    
    if (!cap.isOpened()) {
        std::cerr << "Error: Failed to open camera!" << std::endl;
        running = false;
        frameQueue.stop();
        return;
    }

    cv::Mat frame;
    int frame_count = 0;
    auto last_time = std::chrono::steady_clock::now();

    while (running) {
        if (!cap.read(frame)) {
            std::cerr << "Error: Failed to read frame!" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        if (frame.empty()) {
            std::cerr << "Warning: Empty frame received!" << std::endl;
            continue;
        }

        // Push frame to queue
        frameQueue.push(frame);
        frame_count++;

        // Calculate FPS
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            current_time - last_time).count();
        
        if (elapsed >= 1000) {
            capture_fps = frame_count;
            frame_count = 0;
            last_time = current_time;
        }
    }

    cap.release();
    std::cout << "Capture thread exiting" << std::endl;
}

int main(int argc, char** argv) {
    std::cout << "========================================" << std::endl;
    std::cout << "OpenCV + GStreamer + V4L2 Multi-threading Demo" << std::endl;
    std::cout << "========================================" << std::endl;

    // Parameter configuration
    std::string device = "/dev/video0";
    int width = 1920;
    int height = 1080;
    int fps = 30;
    size_t queue_size = 5;

    // Parse command line arguments
    if (argc >= 2) device = argv[1];
    if (argc >= 3) width = std::atoi(argv[2]);
    if (argc >= 4) height = std::atoi(argv[3]);
    if (argc >= 5) fps = std::atoi(argv[4]);
    if (argc >= 6) queue_size = std::atoi(argv[5]);

    std::cout << "Configuration:" << std::endl;
    std::cout << "  Device: " << device << std::endl;
    std::cout << "  Resolution: " << width << "x" << height << std::endl;
    std::cout << "  FPS: " << fps << " FPS" << std::endl;
    std::cout << "  Queue Size: " << queue_size << std::endl;
    std::cout << std::endl;

    // Build GStreamer Pipeline
    std::string gst_pipeline = "v4l2src device=" + device + " min-buffers=2 io-mode=mmap " +
                               " ! video/x-raw, format=NV12, width=" + std::to_string(width) + 
                               ", height=" + std::to_string(height) + 
                               ", framerate=" + std::to_string(fps) + "/1" +
                               " ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";

    // Create frame queue
    FrameQueue frameQueue(queue_size);

    // Atomic variables for inter-thread communication
    std::atomic<bool> running(true);
    std::atomic<int> capture_fps(0);
    std::atomic<int> display_fps(0);

    // Set environment variables for RGA hardware acceleration
    setenv("GST_VIDEO_CONVERT_USE_RGA", "1", 1);
    setenv("GST_VIDEO_FLIP_USE_RGA", "1", 1);

    // Start capture thread
    std::thread capture_thread(captureThread, std::ref(frameQueue), 
                               gst_pipeline, std::ref(running), 
                               std::ref(capture_fps));

    // Wait a moment to ensure capture thread initialization
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    const std::string window_name = "V4L2 Camera Stream";
    cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);

    cv::Mat frame;
    int frame_count = 0;
    auto last_time = std::chrono::steady_clock::now();

    while (running && frameQueue.is_running()) {
        if (!frameQueue.pop(frame, 1000)) {
            if (!running) break;
            continue;
        }

        if (frame.empty()) {
            continue;
        }

        // Display frame
        cv::imshow(window_name, frame);

        frame_count++;

        // Calculate display FPS
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            current_time - last_time).count();
        
        if (elapsed >= 1000) {
            display_fps = frame_count;
            frame_count = 0;
            last_time = current_time;
            
            // Print FPS information to console (overwrite previous line)
            std::cout << "\r[FPS] Capture: " << capture_fps.load() 
                      << " | Display: " << display_fps.load() 
                      << " | Queue: " << frameQueue.size() 
                      << "  " << std::flush;
        }

        // Check key press
        int key = cv::waitKey(1);
        if (key == 'q' || key == 'Q' || key == 27) { // 'q' or ESC key to exit
            std::cout << std::endl << "Exit key detected..." << std::endl;
            running = false;
            break;
        }
    }

    cv::destroyAllWindows();
    
    // Stop queue and wait for capture thread to finish
    frameQueue.stop();
    capture_thread.join();

    return 0;
}