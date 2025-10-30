cmake_minimum_required(VERSION 3.31)
project(cpp_coretypes)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)
include_directories(optional)

add_executable(cpp_coretypes main.cpp)
