cmake_minimum_required(VERSION 3.9)
project(readbin)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -s -O3")
set(CMAKE_EXE_LINKER_FLAGS -static)


set(GCC_COVERAGE_COMPILE_FLAGS "-Ofast")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

add_executable(readbin main.cpp ShannonFano.cpp ShannonFano.h Huffman.cpp Huffman.h LZ77.cpp LZ77.h FileIO.h FileIO.cpp)