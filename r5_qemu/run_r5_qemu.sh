#!/bin/bash

set -e

script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
qemu_dts_repo_dir="${script_dir}/qemu-devicetrees-xilinx-v2020.1"
zcu102_dtb_path="${qemu_dts_repo_dir}/LATEST/MULTI_ARCH/zcu102-arm.dtb"

cd "${qemu_dts_repo_dir}"
make

# looks like qemu-system-aarch64 is also used for running on the R5
# https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/821854273/Running+Bare+Metal+Applications+on+QEMU
# https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/821952548/Chapter+3+-+Developing+with+QEMU+Virtual+Machines
/tools/Xilinx/Vitis/2020.1/data/emulation/qemu/unified_qemu/sysroots/x86_64-petalinux-linux/usr/bin/qemu-system-aarch64 \
-M arm-generic-fdt \
-serial mon:stdio \
-device loader,file=<baremetal_for_zynqmp_r5.elf>,cpu-num=4 \
-device loader,addr=0XFF5E023C,data=0x80088fde,data-len=4 \
-device loader,addr=0xff9a0000,data=0x80000218,data-len=4 \
-hw-dtb "${zcu102_dtb_path}" \
-m 4048 \
-display none
