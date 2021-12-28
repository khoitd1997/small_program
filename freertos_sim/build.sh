#!/bin/bash

set -e

script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
src_dir="${script_dir}/src"
build_dir="${script_dir}/build"

mkdir -p "${build_dir}"
cd "${build_dir}"

cmake "${src_dir}" && cmake --build .

./freertos_posix_demo