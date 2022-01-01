cmake_minimum_required(VERSION 3.16)
project(barectf_lttng_linux_platform_proj C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(common_compile_flags -g -Wall)

add_library(
  barectf_lttng_linux_platform
  "barectf_function_instrument.cpp" "barectf_utils.cpp"
  "barectf_platform.cpp")
target_include_directories(barectf_lttng_linux_platform
                           PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_options(barectf_lttng_linux_platform PRIVATE -g -Wall)
# barectf_lttng_lib should already be added from the top level CMake File
target_link_libraries(barectf_lttng_linux_platform PUBLIC pthread
                                                          barectf_lttng_lib)
