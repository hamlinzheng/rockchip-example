cmake_minimum_required(VERSION 3.10)
project(VideoCapture)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(video_capture main.cpp)
target_link_libraries(video_capture ${OpenCV_LIBS})

message(STATUS "OpenCV library status:")
message(STATUS "  config: ${OpenCV_DIR}")
message(STATUS "  version: ${OpenCV_VERSION}")
message(STATUS "  libraries: ${OpenCV_LIBS}")
message(STATUS "  include path: ${OpenCV_INCLUDE_DIRS}")
