cmake_minimum_required(VERSION 3.16)
project(test_cmake_app)

add_executable(test_cmake_app_exe main.cpp)
target_include_directories(test_cmake_app_exe
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(test_cmake_app_exe PUBLIC -g -finstrument-functions)
