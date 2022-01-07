#!/bin/bash

set -e

script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
build_dir="${script_dir}/build"

freertos_platform_test_path="${script_dir}/../freertos_platform_test"

splitted_trace_dir="${script_dir}/splitted_traces"

usr_trace_dir="${freertos_platform_test_path}/trace/barectf_user_trace"
usr_trace_path="${usr_trace_dir}/shared_trace"
usr_trace_splitted_dir="${splitted_trace_dir}/barectf_user_trace"

kernel_trace_dir="${freertos_platform_test_path}/trace/barectf_kernel_trace"
kernel_trace_path="${kernel_trace_dir}/shared_trace"
kernel_trace_splitted_dir="${splitted_trace_dir}/barectf_kernel_trace"

mkdir -p "${build_dir}"
cd "${build_dir}"
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug .. && cmake --build .

# bash "${freertos_platform_test_path}/run_test.sh"

rm -rf "${splitted_trace_dir}"
mkdir -p "${splitted_trace_dir}" "${usr_trace_splitted_dir}" "${kernel_trace_splitted_dir}"
cp "${usr_trace_dir}/metadata" "${usr_trace_splitted_dir}"
cp "${kernel_trace_dir}/metadata" "${kernel_trace_splitted_dir}"

# gdb ./trace_splitter_test
./trace_splitter_test "${usr_trace_path}" "${usr_trace_splitted_dir}"

# echo "------------------KERNEL TRACE-------------------------"
# babeltrace2 "${kernel_trace_splitted_dir}"
# echo ""

echo "------------------USER TRACE-------------------------"
babeltrace2 "${usr_trace_splitted_dir}"