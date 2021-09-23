SET(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm64)

set(triple aarch64-buildroot-linux-gnu)

set(CMAKE_C_COMPILER clang)
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_COMPILER_TARGET ${triple})

link_libraries(debuginfod)
add_link_options(
-fuse-ld=lld
-L /home/kd/Downloads/bcc_compile/clang-aarch64/aarch64-linux-gnu/lib
)

set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
set(CMAKE_C_COMPILER_LAUNCHER ccache)

include_directories(SYSTEM 
/home/kd/Downloads/bcc_compile/clang-aarch64/aarch64-linux-gnu/include
/home/kd/Downloads/bcc_compile/clang-aarch64/aarch64-linux-gnu/include/c++/v1
)

set(COMMON_FLAG " -march=armv8-a -mcpu=cortex-a53 ")

set(CMAKE_SYSROOT /home/kd/Downloads/buildroot-2021.05/output/host/aarch64-buildroot-linux-gnu/sysroot)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAG}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --stdlib=libc++ ${COMMON_FLAG}")

list(PREPEND CMAKE_FIND_ROOT_PATH "/home/kd/Downloads/bcc_compile/clang-aarch64/aarch64-linux-gnu")
list(PREPEND CMAKE_FIND_ROOT_PATH "/home/kd/Downloads/buildroot-2021.05/output/host/aarch64-buildroot-linux-gnu/sysroot/usr")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
