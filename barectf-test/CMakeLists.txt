cmake_minimum_required(VERSION 3.16)
project(test_barectf_app C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(common_compile_flags -g -Wall -Werror)

set(bare_ctf_generated_src "${CMAKE_CURRENT_SOURCE_DIR}/barectf_generated_src")
add_library(
  bare_ctf_lib
  barectf_function_instrument.cpp barectf_utils.cpp
  barectf-platform-linux-fs.cpp ${bare_ctf_generated_src}/barectf.c)
target_include_directories(bare_ctf_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                               ${bare_ctf_generated_src})
target_compile_options(bare_ctf_lib PUBLIC ${common_compile_flags})
target_link_libraries(bare_ctf_lib PUBLIC pthread)

add_executable(test_barectf_app_exe main.cpp)
target_link_libraries(test_barectf_app_exe PUBLIC bare_ctf_lib)
target_compile_options(test_barectf_app_exe PUBLIC ${common_compile_flags}
                                                   -finstrument-functions)
add_custom_command(
  TARGET test_barectf_app_exe
  POST_BUILD
  COMMAND nm -C $<TARGET_FILE:test_barectf_app_exe> >
          ${CMAKE_BINARY_DIR}/symbols.txt)
