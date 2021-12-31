#!/bin/bash

set -e

# TODO: Try qemu-r5
# looks like qemu-system-aarch64 is also used for running on the R5
# /tools/Xilinx/Vitis/2020.1/data/emulation/qemu/unified_qemu/sysroots/x86_64-petalinux-linux/usr/bin/qemu-system-aarch64
# https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/821854273/Running+Bare+Metal+Applications+on+QEMU
# https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/821952548/Chapter+3+-+Developing+with+QEMU+Virtual+Machines
/tools/Xilinx/Vitis/2020.1/data/emulation/qemu/unified_qemu/sysroots/x86_64-petalinux-linux/usr/bin/qemu-system-aarch64 \
-M arm-generic-fdt \
-serial mon:stdio \
-device loader,file=<baremetal_for_zynqmp_r5.elf>,cpu-num=4 \
-device loader,addr=0XFF5E023C,data=0x80088fde,data-len=4 \
-device loader,addr=0xff9a0000,data=0x80000218,data-len=4 \
-hw-dtb <device tree binary for Versal> \
-m 4048 \
-display none
