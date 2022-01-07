cmake_minimum_required(VERSION 3.16)
project(trace_splitter_proj CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(trace_splitter_test trace_splitter_test_main.cpp)
target_link_libraries(trace_splitter_test PUBLIC babeltrace-ctf babeltrace)
target_compile_options(trace_splitter_test PUBLIC -g -Wall)
