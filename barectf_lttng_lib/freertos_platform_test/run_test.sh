#!/bin/bash

set -e


script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
build_dir="${script_dir}/build"
trace_dir="${script_dir}/trace"
kernel_trace_dir="${trace_dir}/barectf_kernel_trace"
user_trace_dir="${trace_dir}/barectf_user_trace"
src_dir="${script_dir}"

# rm -rf "${trace_dir}"
# NOTE: It's very important to remove the previous traces, seem like without proper removal,
# the later traces are corrupted
mkdir -p "${trace_dir}" "${user_trace_dir}" "${kernel_trace_dir}"
find "${kernel_trace_dir}" ! -name 'metadata' -type f -exec rm -f {} +
find "${user_trace_dir}" ! -name 'metadata' -type f -exec rm -f {} +
sync

# rm -rf "${build_dir}"
mkdir -p "${build_dir}"
cd "${build_dir}"
cmake -G "Ninja" -DTRACE_ROOT_DIR="${trace_dir}" -DCMAKE_BUILD_TYPE=Debug "${src_dir}" && cmake --build .
# cmake -G "Ninja" -DTRACE_ROOT_DIR="${trace_dir}" -DCMAKE_BUILD_TYPE=Release "${src_dir}" && cmake --build .

# gdb ./fbarectf_lttng_freertos_platform_test
export TRACE_ROOT_DIR="${trace_dir}"
./barectf_lttng_freertos_platform_test
# sleep 1
# gdb ./barectf_lttng_freertos_platform_test

echo "Reading trace using babeltrace"

echo "------------------KERNEL TRACE-------------------------"
babeltrace2 "${kernel_trace_dir}"
echo ""

echo "------------------USER TRACE-------------------------"
babeltrace2 "${user_trace_dir}"