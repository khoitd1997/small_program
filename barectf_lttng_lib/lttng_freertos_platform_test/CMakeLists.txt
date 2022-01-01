cmake_minimum_required(VERSION 3.16)
project(barectf_lttng_freertos_platform_test_proj CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../lib"
                 "${CMAKE_BINARY_DIR}/barectf_lttng_lib")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../freertos_platform"
                 "${CMAKE_BINARY_DIR}/barectf_lttng_freertos_platform")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../freertos/freertos_linux_lib"
                 "${CMAKE_BINARY_DIR}/freertos_linux_lib")

add_executable(barectf_lttng_freertos_platform_test main.cpp console.cpp)
target_include_directories(barectf_lttng_freertos_platform_test
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(barectf_lttng_freertos_platform_test PUBLIC -g -Wall)
target_link_libraries(
  barectf_lttng_freertos_platform_test
  PUBLIC barectf_lttng_lib barectf_lttng_freertos_platform freertos_lib)

add_custom_command(
  TARGET barectf_lttng_freertos_platform_test
  POST_BUILD
  COMMAND nm -C $<TARGET_FILE:barectf_lttng_freertos_platform_test> >
          ${CMAKE_BINARY_DIR}/symbols.txt)
