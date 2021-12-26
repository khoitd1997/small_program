#!/bin/bash

root_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
buildroot_dir="${root_dir}/buildroot-2021.08.1"

configs_path="${root_dir}/configs"
buildroot_defconfig_path="${configs_path}/buildroot_x86_64_defconfig"
kernel_defconfig_path="${configs_path}/kernel_x86_64_defconfig"

buildroot_out_images_dir="${buildroot_dir}/output/images"
buildroot_initrd_out_path="${buildroot_out_images_dir}/rootfs.cpio.gz"
buildroot_kernel_vmlinux_out_path="${buildroot_out_images_dir}/bzImage"