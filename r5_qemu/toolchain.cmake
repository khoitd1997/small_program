set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)
set(CMAKE_CROSSCOMPILING 1)

set(VITIS_INSTALL_DIR "/tools/Xilinx/Vitis/2020.1")
set(TOOLCHAIN_PATH "${VITIS_INSTALL_DIR}/gnu/armr5/lin/gcc-arm-none-eabi/bin")

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CROSS_COMPILE armr5-none-eabi-)

set(CMAKE_C_COMPILER "${TOOLCHAIN_PATH}/${CROSS_COMPILE}gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PATH}/${CROSS_COMPILE}g++")
set(CMAKE_AR
    "${TOOLCHAIN_PATH}/${CROSS_COMPILE}ar"
    CACHE STRING "")
set(CMAKE_LINKER
    "${TOOLCHAIN_PATH}/${CROSS_COMPILE}g++"
    CACHE FILEPATH "The toolchain linker command " FORCE)

set(CMAKE_OBJCOPY
    "${TOOLCHAIN_PATH}/${CROSS_COMPILE}objcopy"
    CACHE FILEPATH "The toolchain objcopy command " FORCE)

set(CMAKE_NM
    "${TOOLCHAIN_PATH}/${CROSS_COMPILE}nm"
    CACHE FILEPATH "The toolchain nm command " FORCE)

add_compile_options(-fmessage-length=0 -mcpu=cortex-r5 -mfloat-abi=hard
                    -mfpu=vfpv3-d16)

set(CMAKE_ASM_FLAGS
    -mcpu=cortex-r5
    CACHE STRING "")
set(CMAKE_EXE_LINKER_FLAGS
    "-mcpu=cortex-r5 -mfloat-abi=hard -mfpu=vfpv3-d16"
    CACHE STRING "")

set(CMAKE_C_ARCHIVE_FINISH true)

add_compile_definitions(ARMR5 UNDEFINE_FILE_OPS)

# The linker require group arg to resolve circular dependency when needing to
# link more libraries, we will have to modify this
set(lib_link_flag
    "-Wl,--start-group,-lxil,-lfreertos,-lgcc,-lc,-lstdc++,--end-group -Wl,--start-group,-lxil,-llwip4,-lgcc,-lc,--end-group -Wl,--start-group,-lxil,-lmetal,-lgcc,-lc,--end-group -Wl,--start-group,-lxil,-lmetal,-lopen_amp,-lgcc,-lc,--end-group"
)

set(CMAKE_C_LINK_EXECUTABLE
    "<CMAKE_C_COMPILER> <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES> ${lib_link_flag}"
)
set(CMAKE_CXX_LINK_EXECUTABLE
    "<CMAKE_CXX_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES> ${lib_link_flag}"
)
