#!/bin/bash

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

cd ${SCRIPT_DIR}

rm -rf install_dir
mkdir install_dir

rm -rf ./bcc/build
mkdir ./bcc/build
cd ./bcc/build

cmake .. -G Ninja -DCMAKE_TOOLCHAIN_FILE=${SCRIPT_DIR}/toolchain.cmake -DENABLE_TESTS=OFF -DENABLE_MAN=OFF -DENABLE_EXAMPLES=OFF -DENABLE_RTTI=ON -DCMAKE_INSTALL_PREFIX=${SCRIPT_DIR}/install_dir
cmake --build .
cmake --build . --target install