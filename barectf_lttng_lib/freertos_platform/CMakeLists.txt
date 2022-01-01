cmake_minimum_required(VERSION 3.16)
project(barectf_lttng_freertos_platform_proj CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_library(barectf_lttng_freertos_platform trace_hook.cpp)

target_include_directories(barectf_lttng_freertos_platform
                           PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_options(barectf_lttng_freertos_platform PUBLIC -g -Wall)

# TODO: Remove the link to barectf_lttng_linux_platform
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../linux_platform"
                 "${CMAKE_BINARY_DIR}/barectf_lttng_linux_platform")
target_link_libraries(
  barectf_lttng_freertos_platform
  PUBLIC barectf_lttng_lib barectf_lttng_linux_platform freertos_lib)

# define this so that FreeRTOS lib includes our trace_hook_macros
set(TRACE_HOOK_MACRO_INCLUDE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}"
    PARENT_SCOPE)
