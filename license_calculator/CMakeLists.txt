cmake_minimum_required(VERSION 3.13)
project(LicenseCalculator LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Werror)
add_subdirectory(license_calculator)

add_executable(license_calculator_exe main.cpp)
target_link_libraries(license_calculator_exe PRIVATE license_calculator)
