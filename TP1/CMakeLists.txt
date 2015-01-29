cmake_minimum_required(VERSION 2.8.4)
project(TP1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_EXE_LINKER_FLAGS "-pthread")

add_executable(TP1 main.cpp ${HEADER_FILES} ${SOURCE_FILES})