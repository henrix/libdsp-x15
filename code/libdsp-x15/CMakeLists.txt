cmake_minimum_required(VERSION 3.5)
project(libdsp_x15)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES library.cpp library.h CallbackResponse.cpp CallbackResponse.h)
add_library(dsp-x15 SHARED ${SOURCE_FILES})

find_package(OpenCL "1.1" REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})
target_link_libraries(dsp-x15 OpenCL)