#!/bin/bash

set -e

# use working directory as where to build the software
build_dir="${PWD}"

echo "Starting Yocto Build"

git clone https://github.com/khoitd1997/yocto_test.git

cd yocto_test
git submodule init
git submodule update
./build_all_config.sh

echo "Yocto Build is Finished"