cmake_minimum_required(VERSION 3.16)
project(freertos_sim CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../barectf_lttng_lib/lib"
                 "${CMAKE_BINARY_DIR}/barectf_lttng_lib")
add_subdirectory(
  "${CMAKE_CURRENT_SOURCE_DIR}/../../barectf_lttng_lib/freertos_platform"
  "${CMAKE_BINARY_DIR}/barectf_lttng_freertos_platform")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/freertos_linux_lib")

add_executable(freertos_posix_demo main.cpp console.cpp)
target_include_directories(freertos_posix_demo
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(freertos_posix_demo PUBLIC -g -Wall)
target_link_libraries(
  freertos_posix_demo PUBLIC barectf_lttng_lib barectf_lttng_freertos_platform
                             freertos_lib)

add_custom_command(
  TARGET freertos_posix_demo
  POST_BUILD
  COMMAND nm -C $<TARGET_FILE:freertos_posix_demo> >
          ${CMAKE_BINARY_DIR}/symbols.txt)
