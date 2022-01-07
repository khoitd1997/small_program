cmake_minimum_required(VERSION 3.16)
project(trace_splitter_proj CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_library(trace_splitter_lib trace_splitter.cpp)
target_compile_options(trace_splitter_lib PUBLIC -g -Wall)
target_include_directories(trace_splitter_lib
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
