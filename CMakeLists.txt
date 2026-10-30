cmake_minimum_required(VERSION 3.20)

project(OpenCVLab)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCV 4.0 REQUIRED COMPONENTS core imgproc highgui videoio imgcodecs features2d calib3d video)

if(OpenCV_FOUND)
    message(STATUS "OpenCV found: ${OpenCV_VERSION}")
else()
    message(FATAL_ERROR "OpenCV not found. Please install OpenCV development libraries.")
endif()

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/*.h"
    "src/*.hpp"
)

add_executable(OpenCVLab ${SOURCE_FILES})

target_link_libraries(OpenCVLab PRIVATE ${OpenCV_LIBS})
