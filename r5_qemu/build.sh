#!/bin/bash

set -e

script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
build_dir="${script_dir}/build"

# rm -rf "${build_dir}"
mkdir -p "${build_dir}"

cd "${build_dir}"
cmake -G "Ninja" -DCMAKE_TOOLCHAIN_FILE="${script_dir}/../r5_bsp_build/r5_toolchain.cmake" -DCMAKE_BUILD_TYPE=Debug .. && cmake --build .