cmake_minimum_required(VERSION 3.16)
project(barectf_lttng_lib_proj C CXX)

find_program(BARECTF_EXE barectf)
if(NOT BARECTF_EXE)
  message(FATAL_ERROR "barectf executable not found in PATH")
endif()

if(NOT DEFINED TRACE_ROOT_DIR)
  set(TRACE_ROOT_DIR "${CMAKE_BINARY_DIR}")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(barectf_build_dir "${CMAKE_BINARY_DIR}")
set(barectf_script_path "${CMAKE_CURRENT_SOURCE_DIR}/build_barectf.sh")
set(barectf_config_dir "${CMAKE_CURRENT_SOURCE_DIR}/barectf_configs")
set(barectf_config_files
    "${barectf_config_dir}/config.yml" "${barectf_config_dir}/env_kernel.yml"
    "${barectf_config_dir}/env_user.yml")

set(barectf_generated_src_dir "${barectf_build_dir}/barectf_generated_src")
set(barectf_generated_src_files
    "${barectf_generated_src_dir}/barectf-bitfield.h"
    "${barectf_generated_src_dir}/barectf.c"
    "${barectf_generated_src_dir}/barectf.h")

add_custom_command(
  COMMAND
    "${barectf_script_path}" --config-dir "${barectf_config_dir}" --build-dir
    "${barectf_build_dir}" --trace-root-dir "${TRACE_ROOT_DIR}"
  DEPENDS ${barectf_config_files}
  OUTPUT ${barectf_generated_src_files}
  VERBATIM)

add_library(barectf_lttng_lib ${barectf_generated_src_files})
target_include_directories(barectf_lttng_lib
                           PUBLIC ${barectf_generated_src_dir})
target_compile_options(barectf_lttng_lib PRIVATE -g -Wall)
