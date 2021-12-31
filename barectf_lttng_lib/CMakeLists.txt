cmake_minimum_required(VERSION 3.16)
project(barectf_lttng_lib_proj C CXX)

find_program(BARECTF_EXE barectf)
if(NOT BARECTF_EXE)
  message(FATAL_ERROR "barectf executable not found in PATH")
endif()

if(NOT DEFINED TRACE_ROOT_DIR)
  set(TRACE_ROOT_DIR "${CMAKE_BINARY_DIR}")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(common_compile_flags -g -Wall)

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

set(kernel_trace_dir "${TRACE_ROOT_DIR}/kernel_trace")
set(user_trace_dir "${TRACE_ROOT_DIR}/user_trace")

add_custom_command(
  COMMAND
    "${barectf_script_path}" --config-dir "${barectf_config_dir}" --build-dir
    "${barectf_build_dir}" --trace-root-dir "${TRACE_ROOT_DIR}"
  DEPENDS ${barectf_config_files}
  OUTPUT ${barectf_generated_src_files}
  VERBATIM)

set(lib_dir "${CMAKE_CURRENT_SOURCE_DIR}/lib")
add_library(
  barectf_lttng_lib
  "${lib_dir}/barectf_function_instrument.cpp" "${lib_dir}/barectf_utils.cpp"
  "${lib_dir}/barectf-platform-linux-fs.cpp" ${barectf_generated_src_files})
target_include_directories(barectf_lttng_lib
                           PUBLIC ${lib_dir} ${barectf_generated_src_dir})
target_compile_options(barectf_lttng_lib PUBLIC ${common_compile_flags})
target_link_libraries(barectf_lttng_lib PUBLIC pthread)

# Test App Build
add_executable(test_barectf_app_exe main.cpp)
target_link_libraries(test_barectf_app_exe PUBLIC barectf_lttng_lib)
target_compile_options(test_barectf_app_exe PUBLIC ${common_compile_flags}
                                                   -finstrument-functions)
add_custom_command(
  TARGET test_barectf_app_exe
  POST_BUILD
  COMMAND nm -C $<TARGET_FILE:test_barectf_app_exe> >
          ${CMAKE_BINARY_DIR}/symbols.txt)
