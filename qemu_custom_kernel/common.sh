#!/bin/bash

root_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
buildroot_dir="${root_dir}/buildroot-2021.08.1"

configs_path="${root_dir}/configs"
buildroot_defconfig_path="${configs_path}/buildroot_x86_64_defconfig"
kernel_defconfig_path="${configs_path}/kernel_x86_64_defconfig"