#!/bin/bash

set -e

script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
cd "${script_dir}"

generated_meta_dir="${script_dir}/generate_trace_metadata"
rm -rf "${generated_meta_dir}"
mkdir -p "${generated_meta_dir}"

generated_src_dir="${script_dir}/barectf_generated_src"
rm -rf "${generated_src_dir}"
mkdir "${generated_src_dir}"
cd "${generated_src_dir}"
barectf generate -m "${generated_meta_dir}" "${script_dir}/config.yml"

cd "${script_dir}"
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build .

rm -rf trace
mkdir trace
./test_barectf_app_exe lorem ipsum nulla dolore consequat

# need metadata to read the trace
cp "${generated_meta_dir}/metadata" trace
babeltrace2 trace