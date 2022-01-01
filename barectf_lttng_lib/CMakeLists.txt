cmake_minimum_required(VERSION 3.16)
project(barectf_lttng_test_proj C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/linux_platform")

add_executable(test_barectf_app_exe test_main.cpp)
target_link_libraries(test_barectf_app_exe PUBLIC barectf_lttng_lib
                                                  barectf_lttng_linux_platform)
target_compile_options(test_barectf_app_exe PUBLIC -g -Wall
                                                   -finstrument-functions)
add_custom_command(
  TARGET test_barectf_app_exe
  POST_BUILD
  COMMAND nm -C $<TARGET_FILE:test_barectf_app_exe> >
          ${CMAKE_BINARY_DIR}/symbols.txt)
