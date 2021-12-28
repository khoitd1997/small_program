#!/bin/bash

set -e

script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

cd "${script_dir}"
mkdir -p build
cd build
cmake .. && cmake --build .
