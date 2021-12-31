#!/bin/bash

set -e

source /tools/Xilinx/Vitis/2020.1/settings64.sh

script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
vitis_build_dir="${script_dir}/vitis_build"

rm -rf "${vitis_build_dir}"
mkdir "${vitis_build_dir}"

cd "${vitis_build_dir}"
cp "${script_dir}/zcu102.xsa" .
xsct "${script_dir}/build.tcl"