﻿cmake_minimum_required (VERSION 3.8)

add_executable(rptr_test "rptr_test.cpp")

add_executable(exstream_test "zstream_test.cpp")
target_link_libraries(exstream_test exstream)
