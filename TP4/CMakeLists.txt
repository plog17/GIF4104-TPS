cmake_minimum_required(VERSION 2.8.4)
project(TP4)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

FIND_PACKAGE(OpenCL REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${OPENCL_INCLUDE_DIR})

file(GLOB HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h*")
file(GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(TP4 src/main.cpp ${HEADER_FILES} ${SOURCE_FILES})
