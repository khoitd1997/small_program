cmake_minimum_required(VERSION 3.16)
project(test_barectf_app C CXX)

set(bare_ctf_generated_src "${CMAKE_CURRENT_SOURCE_DIR}/barectf_generated_src")
add_executable(test_barectf_app_exe main.cpp barectf-platform-linux-fs.cpp
                                    ${bare_ctf_generated_src}/barectf.c)
target_include_directories(
  test_barectf_app_exe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                              ${bare_ctf_generated_src})
target_compile_options(test_barectf_app_exe PUBLIC -g)
