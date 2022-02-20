cmake_minimum_required(VERSION 3.20)
project(r5_test_build C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(XSA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/zcu102.xsa")
set(CUSTOM_VITIS_REPO_PATH "${CMAKE_CURRENT_SOURCE_DIR}/my_vitis_repo")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../r5_bsp_build"
                 "${CMAKE_BINARY_DIR}/r5_bsp_build")

add_library(test_lib "${CMAKE_CURRENT_SOURCE_DIR}/test_lib/lib_src.cpp")
target_include_directories(
  test_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/test_lib"
                  "${R5_BSP_EXPORT_INCLUDE_DIR}")

set(app_src "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(openamp_src "${app_src}/openamp")
add_executable(
  r5_test.elf
  "${app_src}/main.cpp" "${openamp_src}/platform_info.c"
  "${openamp_src}/rsc_table.c" "${openamp_src}/zynqmp_r5_a53_rproc.c")

add_dependencies(r5_test.elf r5_bsp_lib_target)
target_include_directories(r5_test.elf PUBLIC ${R5_BSP_EXPORT_INCLUDE_DIR})
target_compile_options(r5_test.elf PUBLIC -g -Wall)
target_link_libraries(r5_test.elf test_lib)

set(linker_script_path "${CMAKE_CURRENT_SOURCE_DIR}/lscript.ld")
set_target_properties(r5_test.elf PROPERTIES LINK_DEPENDS
                                             "${linker_script_path}")
target_link_options(r5_test.elf PRIVATE "-Wl,-T" "-Wl,${linker_script_path}"
                    "-L${R5_BSP_EXPORT_LIB_DIR}")

add_custom_command(
  TARGET r5_test.elf
  POST_BUILD
  COMMAND "${CMAKE_NM}" -C $<TARGET_FILE:r5_test.elf> >
          "${CMAKE_BINARY_DIR}/symbols.txt")
