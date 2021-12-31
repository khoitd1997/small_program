#!/bin/bash

set -e


script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
build_dir="${script_dir}/build"
trace_dir="${script_dir}/trace"
src_dir="${script_dir}/src"

rm -rf "${trace_dir}"
mkdir "${trace_dir}"
kernel_trace_dir="${trace_dir}/kernel_trace"
user_trace_dir="${trace_dir}/user_trace"

rm -rf "${build_dir}"
mkdir -p "${build_dir}"
cd "${build_dir}"
cmake -G "Ninja" -DTRACE_ROOT_DIR="${trace_dir}" -DCMAKE_BUILD_TYPE=Debug "${src_dir}" && cmake --build .

# gdb ./freertos_posix_demo
TRACE_ROOT_DIR="${trace_dir}" ./freertos_posix_demo

echo "Reading trace using babeltrace"
babeltrace2 "${kernel_trace_dir}"