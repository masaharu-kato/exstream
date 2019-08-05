cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
include_directories("src")

project("exstream_test")

add_subdirectory("test")
add_subdirectory("src")