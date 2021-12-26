#!/bin/bash

set -e

script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

buildroot_dir="${script_dir}/buildroot-2021.08.1"
kernel_path="vmlinuz-5.4.0-21"
initrd_path="/boot/initrd.img-5.4.0-21-workload"

if [ ! -f "${kernel_path}" ]; then
    echo "Invalid kernel path"
    exit 1
fi


# references: https://ubuntu.com/server/docs/virtualization-qemu
sudo qemu-system-x86_64 -M test_computer -cpu host -m 1024 -enable-kvm -serial mon:stdio -nographic -display curses -append 'console=ttyS0,115200,8n1' -kernel "${kernel_path}" -initrd "${initrd_path}"