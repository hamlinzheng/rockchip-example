# CMakeLists.txt for OpenCL Native Vector Computation Example
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(opencl_native VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find OpenCL package
find_package(OpenCL REQUIRED)

if(NOT OpenCL_FOUND)
    message(FATAL_ERROR "OpenCL not found! Please install OpenCL SDK.")
endif()

# Display OpenCL information
message(STATUS "OpenCL found: ${OpenCL_FOUND}")
message(STATUS "OpenCL include directory: ${OpenCL_INCLUDE_DIRS}")
message(STATUS "OpenCL libraries: ${OpenCL_LIBRARIES}")

# Add executable
add_executable(opencl_native main.cpp)

# Include OpenCL headers
target_include_directories(opencl_native PRIVATE ${OpenCL_INCLUDE_DIRS})

# Link OpenCL libraries
target_link_libraries(opencl_native PRIVATE ${OpenCL_LIBRARIES})

# Compiler flags for better warnings and optimization
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(opencl_native PRIVATE 
        -Wall 
        -Wextra 
        -O2
    )
endif()

# Set output directory
set_target_properties(opencl_native PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Print build configuration
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ flags: ${CMAKE_CXX_FLAGS}")
