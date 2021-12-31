#!/bin/bash

set -e

script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
build_dir="${script_dir}/build"

rm -rf "${build_dir}"
mkdir "${build_dir}"

cd "${build_dir}"
cmake -G "Ninja" -DCMAKE_TOOLCHAIN_FILE="${script_dir}/toolchain.cmake" -DCMAKE_BUILD_TYPE=Debug .. && cmake --build .
# "/tools/Xilinx/Vitis/2020.1/bin/xsct" "${script_dir}/build_bsp.tcl" "${script_dir}/zcu102.xsa"