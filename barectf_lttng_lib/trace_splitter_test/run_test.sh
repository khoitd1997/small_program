#!/bin/bash

set -e

script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
build_dir="${script_dir}/build"

freertos_platform_test_path="${script_dir}/../freertos_platform_test"

splitted_trace_dir="${script_dir}/splitted_traces"

user_trace_dir="${freertos_platform_test_path}/trace/barectf_bundled_user_trace"
user_trace_path="${user_trace_dir}/bundled_trace"
user_trace_splitted_dir="${splitted_trace_dir}/barectf_user_trace"

kernel_trace_dir="${freertos_platform_test_path}/trace/barectf_bundled_kernel_trace"
kernel_trace_path="${kernel_trace_dir}/bundled_trace"
kernel_trace_splitted_dir="${splitted_trace_dir}/barectf_kernel_trace"

mkdir -p "${build_dir}"
cd "${build_dir}"
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug .. && cmake --build .

# bash "${freertos_platform_test_path}/run_test.sh"

rm -rf "${splitted_trace_dir}"
mkdir -p "${splitted_trace_dir}" "${user_trace_splitted_dir}" "${kernel_trace_splitted_dir}"
cp "${user_trace_dir}/metadata" "${user_trace_splitted_dir}"
cp "${kernel_trace_dir}/metadata" "${kernel_trace_splitted_dir}"

# gdb ./trace_splitter_test
./trace_splitter_test "${user_trace_path}" "${user_trace_splitted_dir}"
./trace_splitter_test "${kernel_trace_path}" "${kernel_trace_splitted_dir}"

echo "------------------SPLITTED KERNEL TRACE-------------------------"
babeltrace2 "${kernel_trace_splitted_dir}"
echo ""

echo "------------------SPLITTED USER TRACE-------------------------"
babeltrace2 "${user_trace_splitted_dir}"