set elf_file_path "[lindex $argv 0]"

connect -url tcp:127.0.0.1:3121
source /tools/Xilinx/Vitis/2020.1/scripts/vitis/util/zynqmp_utils.tcl

targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -nocase -filter {name =~"RPU*"}
clear_rpu_reset
enable_split_mode

targets -set -nocase -filter {name =~"APU*"}
loadhw -hw /home/kd/r5_qemu_workspace/r5_platform/export/r5_platform/hw/design_1_wrapper.xsa -mem-ranges [list {0x80000000 0xbfffffff} {0x400000000 0x5ffffffff} {0x1000000000 0x7fffffffff}] -regs
configparams force-mem-access 1

targets -set -nocase -filter {name =~"APU*"}
set mode [expr [mrd -value 0xFF5E0200] & 0xf]

targets -set -nocase -filter {name =~ "*R5*#0"}
rst -processor

dow /home/kd/r5_qemu_workspace/r5_platform/export/r5_platform/sw/r5_platform/boot/fsbl.elf
set bp_55_7_fsbl_bp [bpadd -addr &XFsbl_Exit]
con -block -timeout 60
bpremove $bp_55_7_fsbl_bp

targets -set -nocase -filter {name =~ "*R5*#0"}
rst -processor
dow $elf_file_path
configparams force-mem-access 0