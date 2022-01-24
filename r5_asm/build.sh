#!/bin/bash

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

cd "${SCRIPT_DIR}"

/tools/Xilinx/Vitis/2020.1/gnu/armr5/lin/gcc-arm-none-eabi/bin/armr5-none-eabi-g++ -S -Wa,-adhln -fverbose-asm -fno-asynchronous-unwind-tables -fno-exceptions -fno-rtti main.cpp