#!/bin/bash

set -e

script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
build_dir="${script_dir}/build"
trace_dir="${script_dir}/trace"
kernel_trace_dir="${trace_dir}/barectf_kernel_trace"
user_trace_dir="${trace_dir}/barectf_user_trace"

mkdir -p "${trace_dir}" "${user_trace_dir}" "${kernel_trace_dir}"
find "${kernel_trace_dir}" ! -name 'metadata' -type f -exec rm -f {} +
find "${user_trace_dir}" ! -name 'metadata' -type f -exec rm -f {} +
sync

mkdir -p "${build_dir}"
cd "${build_dir}"
cmake -G "Ninja" -DTRACE_ROOT_DIR="${trace_dir}" -DCMAKE_BUILD_TYPE=Debug .. && cmake --build .

# gdb ./barectf_lttng_linux_platform_test lorem ipsum nulla dolore consequat
TRACE_ROOT_DIR="${trace_dir}" ./barectf_lttng_linux_platform_test lorem ipsum nulla dolore consequat

echo "Reading trace using babeltrace"
babeltrace2 "${user_trace_dir}"