#!/bin/bash

set -e

# use working directory as where to build the software
build_dir="${PWD}"

echo "Starting Yocto Build"

git lfs install --skip-repo

rm -rf yocto_test
git clone https://github.com/khoitd1997/yocto_test.git

cd yocto_test
git submodule init
git submodule update
# ./build_all_config.sh
./set_build_config.sh debug
./build.sh

echo "Yocto Build is Finished"