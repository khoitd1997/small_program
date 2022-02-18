cmake_minimum_required(VERSION 3.20)
project(r5_test_build C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(linker_script_path "${CMAKE_CURRENT_SOURCE_DIR}/lscript.ld")

set(vitis_build_dir "${CMAKE_BINARY_DIR}/vitis_build")
set(r5_workspace_dir "${vitis_build_dir}/r5_workspace")
set(r5_platform_dir "${r5_workspace_dir}/r5_platform")
set(r5_bsp_export_dir
    "${r5_platform_dir}/export/r5_platform/sw/r5_platform/r5_freertos")
set(r5_bsp_export_lib_dir "${r5_bsp_export_dir}/bsplib/lib")
set(r5_bsp_export_include_dir "${r5_bsp_export_dir}/bspinclude/include")
set(r5_lib_list
    "${r5_bsp_export_lib_dir}/libfreertos.a"
    "${r5_bsp_export_lib_dir}/liblwip4.a"
    "${r5_bsp_export_lib_dir}/libmetal.a"
    "${r5_bsp_export_lib_dir}/libopen_amp.a"
    "${r5_bsp_export_lib_dir}/libxil.a")

set(custom_vitis_repo_path "${CMAKE_CURRENT_SOURCE_DIR}/my_vitis_repo")
set(xsa_path "${CMAKE_CURRENT_SOURCE_DIR}/zcu102.xsa")

set(tcl_script_path "${CMAKE_CURRENT_SOURCE_DIR}/build_bsp.tcl")

add_custom_command(
  COMMAND ${CMAKE_COMMAND} -E remove_directory "${vitis_build_dir}"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${vitis_build_dir}"
  COMMAND "${VITIS_INSTALL_DIR}/bin/xsct" "${tcl_script_path}" "${xsa_path}"
          "${custom_vitis_repo_path}"
  WORKING_DIRECTORY "${vitis_build_dir}"
  DEPENDS "${tcl_script_path}"
  OUTPUT ${r5_lib_list}
  VERBATIM)
add_custom_target(r5_bsp_lib_target DEPENDS ${r5_lib_list})

add_custom_target(
  show_r5_bsp_in_vitis
  COMMAND "${VITIS_INSTALL_DIR}/bin/vitis" -lp "${custom_vitis_repo_path}"
          -workspace "${r5_workspace_dir}"
  WORKING_DIRECTORY "${vitis_build_dir}"
  VERBATIM)
add_dependencies(show_r5_bsp_in_vitis r5_bsp_lib_target)

add_library(test_lib "${CMAKE_CURRENT_SOURCE_DIR}/test_lib/lib_src.cpp")
target_include_directories(test_lib
                           PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/test_lib")

set(app_src "${CMAKE_CURRENT_SOURCE_DIR}/src")
add_executable(r5_test.elf "${app_src}/main.cpp")
add_dependencies(r5_test.elf r5_bsp_lib_target)
target_include_directories(r5_test.elf PUBLIC ${r5_bsp_export_include_dir})
target_compile_options(r5_test.elf PUBLIC -g -Wall)
target_link_libraries(r5_test.elf test_lib)

set_target_properties(r5_test.elf PROPERTIES LINK_DEPENDS
                                             "${linker_script_path}")

target_link_options(r5_test.elf PRIVATE "-Wl,-T" "-Wl,${linker_script_path}"
                    "-L${r5_bsp_export_lib_dir}")

add_custom_command(
  TARGET r5_test.elf
  POST_BUILD
  COMMAND "${CMAKE_NM}" -C $<TARGET_FILE:r5_test.elf> >
          "${CMAKE_BINARY_DIR}/symbols.txt")
