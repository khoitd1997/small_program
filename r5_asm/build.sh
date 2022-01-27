#!/bin/bash

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

cd "${SCRIPT_DIR}"

# NOTE: GCC ARM Intrinsics
# https://gcc.gnu.org/onlinedocs/gcc/ARM-C-Language-Extensions-_0028ACLE_0029.html

# NOTE: Look for vfpv3-d16 in the arm7-r reference manual
# it indicates that this is the VFPv3 unit with 16 double word

# NOTE: compiler won't generate vectorize call with this:
# -mfpu=vfpv3-d16
# the below gets vectorization
# -mfpu=neon-vfpv3

ARCH_FLAG="-mcpu=cortex-r5 -mfloat-abi=hard"
NICE_ASM_FLAG="-Wa,-adhln -fverbose-asm -fno-asynchronous-unwind-tables -fno-exceptions -fno-rtti "
VECTORIZE_FLAG="-O1 -ftree-vectorize -fopt-info-vec-all -fopt-info"
/tools/Xilinx/Vitis/2020.1/gnu/armr5/lin/gcc-arm-none-eabi/bin/armr5-none-eabi-g++ -S \
    ${ARCH_FLAG} \
    ${NICE_ASM_FLAG} \
    ${VECTORIZE_FLAG} \
    -mfpu=neon-vfpv3 \
    main.cpp