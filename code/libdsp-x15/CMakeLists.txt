cmake_minimum_required(VERSION 3.5)
project(libdsp-x15)

if(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")
    add_definitions(/W2)
endif()

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES library.cpp library.h DspTask.cpp DspTask.h TaskProcessor.cpp TaskProcessor.h FFT_SP.cpp FFT_SP.h IFFT_SP.cpp IFFT_SP.h FilterBiquadSP.cpp FilterBiquadSP.h DspTaskFactory.cpp DspTaskFactory.h)
add_library(dsp-x15 SHARED ${SOURCE_FILES})

find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})
target_link_libraries(dsp-x15 OpenCL)
