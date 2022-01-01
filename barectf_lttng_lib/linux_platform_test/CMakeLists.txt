cmake_minimum_required(VERSION 3.16)
project(barectf_lttng_linux_platform_test_proj C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../lib"
                 "${CMAKE_BINARY_DIR}/barectf_lttng_lib")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../linux_platform"
                 "${CMAKE_BINARY_DIR}/barectf_lttng_linux_platform")

add_executable(barectf_lttng_linux_platform_test test_main.cpp)
target_link_libraries(barectf_lttng_linux_platform_test
                      PUBLIC barectf_lttng_lib barectf_lttng_linux_platform)
target_compile_options(barectf_lttng_linux_platform_test
                       PUBLIC -g -Wall -finstrument-functions)
add_custom_command(
  TARGET barectf_lttng_linux_platform_test
  POST_BUILD
  COMMAND nm -C $<TARGET_FILE:barectf_lttng_linux_platform_test> >
          ${CMAKE_BINARY_DIR}/symbols.txt)
