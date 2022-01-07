#!/bin/bash

set -e

script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
build_dir="${script_dir}/build"

mkdir -p "${build_dir}"
cd "${build_dir}"
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug .. && cmake --build .

# gdb ./trace_splitter_test
./trace_splitter_test

# echo "Reading trace using babeltrace"
# babeltrace2 "${user_trace_dir}"