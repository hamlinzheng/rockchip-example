cmake_minimum_required(VERSION 3.10)
project(camera_gstreamer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compilation options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")

# Find OpenCV
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    message(STATUS "Found OpenCV version: ${OpenCV_VERSION}")
    message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")
    message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
else()
    message(FATAL_ERROR "OpenCV not found!")
endif()

# Find PkgConfig (for GStreamer)
find_package(PkgConfig REQUIRED)

# Find GStreamer
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
pkg_check_modules(GSTREAMER_APP REQUIRED gstreamer-app-1.0)
pkg_check_modules(GSTREAMER_VIDEO REQUIRED gstreamer-video-1.0)

if(GSTREAMER_FOUND)
    message(STATUS "Found GStreamer version: ${GSTREAMER_VERSION}")
    message(STATUS "GStreamer include dirs: ${GSTREAMER_INCLUDE_DIRS}")
    message(STATUS "GStreamer libraries: ${GSTREAMER_LIBRARIES}")
else()
    message(FATAL_ERROR "GStreamer not found!")
endif()

# Find Threads
find_package(Threads REQUIRED)

# Include directories
include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${GSTREAMER_INCLUDE_DIRS}
    ${GSTREAMER_APP_INCLUDE_DIRS}
    ${GSTREAMER_VIDEO_INCLUDE_DIRS}
)

# Link directories
link_directories(
    ${GSTREAMER_LIBRARY_DIRS}
    ${GSTREAMER_APP_LIBRARY_DIRS}
    ${GSTREAMER_VIDEO_LIBRARY_DIRS}
)

# Add executable
add_executable(camera_gstreamer main.cpp)

# Link libraries
target_link_libraries(camera_gstreamer
    ${OpenCV_LIBS}
    ${GSTREAMER_LIBRARIES}
    ${GSTREAMER_APP_LIBRARIES}
    ${GSTREAMER_VIDEO_LIBRARIES}
    Threads::Threads
)

# Install rules
install(TARGETS camera_gstreamer DESTINATION bin)

# Print configuration
message(STATUS "========================================")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "========================================")
