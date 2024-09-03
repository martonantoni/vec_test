cmake_minimum_required(VERSION 3.20)
project(heap LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(heap heap.cpp)
