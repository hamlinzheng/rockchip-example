cmake_minimum_required(VERSION 3.10)
project(opencv_opencl)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})

# Add executables
add_executable(check_opencv check_opencv.cpp)
add_executable(fisheye_undistortion main.cpp)

# Link libraries
target_link_libraries(check_opencv ${OpenCV_LIBS})
target_link_libraries(fisheye_undistortion ${OpenCV_LIBS})

# Print OpenCV information
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
