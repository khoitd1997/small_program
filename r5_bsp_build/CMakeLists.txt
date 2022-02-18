cmake_minimum_required(VERSION 3.20)
project(r5_bsp_project C CXX)

set(vitis_build_dir "${CMAKE_CURRENT_BINARY_DIR}/vitis_build")
set(r5_workspace_dir "${vitis_build_dir}/r5_workspace")
set(r5_platform_dir "${r5_workspace_dir}/r5_platform")
set(r5_bsp_export_dir
    "${r5_platform_dir}/export/r5_platform/sw/r5_platform/r5_freertos")
set(R5_BSP_EXPORT_LIB_DIR
    "${r5_bsp_export_dir}/bsplib/lib"
    PARENT_SCOPE)
set(R5_BSP_EXPORT_INCLUDE_DIR
    "${r5_bsp_export_dir}/bspinclude/include"
    PARENT_SCOPE)
set(r5_lib_list
    "${R5_BSP_EXPORT_LIB_DIR}/libfreertos.a"
    "${R5_BSP_EXPORT_LIB_DIR}/liblwip4.a"
    "${R5_BSP_EXPORT_LIB_DIR}/libmetal.a"
    "${R5_BSP_EXPORT_LIB_DIR}/libopen_amp.a"
    "${R5_BSP_EXPORT_LIB_DIR}/libxil.a")

set(tcl_script_path "${CMAKE_CURRENT_SOURCE_DIR}/build_bsp.tcl")

add_custom_command(
  COMMAND ${CMAKE_COMMAND} -E remove_directory "${vitis_build_dir}"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${vitis_build_dir}"
  COMMAND
    ${CMAKE_COMMAND} -E chdir "${vitis_build_dir}"
    "${VITIS_INSTALL_DIR}/bin/xsct" "${tcl_script_path}" "${XSA_PATH}"
    "${CUSTOM_VITIS_REPO_PATH}"
  DEPENDS "${tcl_script_path}"
  OUTPUT ${r5_lib_list}
  VERBATIM)
add_custom_target(r5_bsp_lib_target DEPENDS ${r5_lib_list})

add_custom_target(
  show_r5_bsp_in_vitis
  COMMAND "${VITIS_INSTALL_DIR}/bin/vitis" -lp "${CUSTOM_VITIS_REPO_PATH}"
          -workspace "${r5_workspace_dir}"
  WORKING_DIRECTORY "${vitis_build_dir}"
  VERBATIM)
add_dependencies(show_r5_bsp_in_vitis r5_bsp_lib_target)
