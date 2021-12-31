cmake_minimum_required(VERSION 3.16)
project(r5_test_build C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(linker_script "${CMAKE_CURRENT_SOURCE_DIR}/lscript.ld")

set(app_src "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(vitis_build_dir "${CMAKE_BINARY_DIR}/vitis_build")
set(r5_bsp_export_dir
    "${vitis_build_dir}/r5_workspace/r5_platform/export/r5_platform/sw/r5_platform/r5_freertos"
)
set(r5_bsp_export_lib_dir "${r5_bsp_export_dir}/bsplib/lib")
set(r5_bsp_export_include_dir "${r5_bsp_export_dir}/bspinclude/include")
set(r5_lib_list
    "${r5_bsp_export_lib_dir}/libfreertos.a"
    "${r5_bsp_export_lib_dir}/liblwip4.a"
    "${r5_bsp_export_lib_dir}/libmetal.a"
    "${r5_bsp_export_lib_dir}/libopen_amp.a"
    "${r5_bsp_export_lib_dir}/libxil.a")

set(vitis_path "/tools/Xilinx/Vitis/2020.1")
set(tcl_script_path "${CMAKE_CURRENT_SOURCE_DIR}/build_bsp.tcl")

add_custom_command(
  COMMAND ${CMAKE_COMMAND} -E make_directory "${vitis_build_dir}"
  COMMAND
    "${vitis_path}/bin/xsct" "${tcl_script_path}"
    "${CMAKE_CURRENT_SOURCE_DIR}/zcu102.xsa"
    "${CMAKE_CURRENT_SOURCE_DIR}/my_vitis_repo"
  WORKING_DIRECTORY "${vitis_build_dir}"
  DEPENDS "${tcl_script_path}"
  OUTPUT ${r5_lib_list}
  VERBATIM)
add_custom_target(r5_bsp_lib_target DEPENDS ${r5_lib_list})

add_library(test_lib "${CMAKE_CURRENT_SOURCE_DIR}/test_lib/lib_src.cpp")
target_include_directories(test_lib
                           PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/test_lib")

add_executable(r5_test.elf "${app_src}/main.cpp")
add_dependencies(r5_test.elf r5_bsp_lib_target)
target_include_directories(r5_test.elf PUBLIC ${r5_bsp_export_include_dir})
# target_compile_definitions(r5_test.elf PUBLIC _WINDOWS_)
target_compile_options(r5_test.elf PUBLIC -g -Wall)
target_link_libraries(r5_test.elf test_lib)

set_target_properties(r5_test.elf PROPERTIES LINK_DEPENDS "${linker_script}")

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-T -Wl,${linker_script} -L${r5_bsp_export_lib_dir}"
)

# add_custom_command( TARGET r5_test.elf COMMAND nm -C
# $<TARGET_FILE:r5_test.elf> > ${CMAKE_BINARY_DIR}/symbols.txt)

# for generating disassembly add_custom_command( TARGET ${TARGET}.elf POST_BUILD
# COMMAND ${CMAKE_OBJDUMP} -S ${TARGET}.elf > ${TARGET}.disasm COMMENT
# "Invoking: Disassemble") foreach(SRC_MODULE test startup dummy)
# add_custom_command( TARGET ${TARGET}.elf PRE_LINK COMMAND ${CMAKE_OBJDUMP} -S
# CMakeFiles/${TARGET}.elf.dir/${SRC_MODULE}.c.obj > ${SRC_MODULE}.s COMMENT
# "Invoking: Disassemble ( CMakeFiles/${TARGET}.elf.dir/${SRC_MODULE}.c.obj)" )
# endforeach()
