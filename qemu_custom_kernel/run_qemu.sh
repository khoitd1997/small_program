#!/bin/bash

set -e

script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
source "${script_dir}/common.sh"

if [ ! -f "${buildroot_kernel_vmlinux_out_path}" ]; then
    echo "Invalid kernel path"
    exit 1
fi


# references: https://ubuntu.com/server/docs/virtualization-qemu
#  -serial mon:stdio
qemu-system-x86_64 -device e1000,netdev=net0 -netdev user,id=net0,hostfwd=tcp::5555-:22 -cpu host -m 4096 -enable-kvm -nographic -display curses -append 'console=ttyS0,115200,8n1' -kernel "${buildroot_kernel_vmlinux_out_path}" -initrd "${buildroot_initrd_out_path}"