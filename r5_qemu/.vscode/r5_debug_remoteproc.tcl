set elf_file_path "[lindex $argv 0]"

connect -url tcp:127.0.0.1:3121
source /tools/Xilinx/Vitis/2020.1/scripts/vitis/util/zynqmp_utils.tcl

exec sshpass -p root scp /home/kd/small_program/r5_qemu/build/default/r5_test.elf root@192.168.1.100:/tmp
exec sshpass -p root ssh root@192.168.1.100 flash_r5.sh /tmp/r5_test.elf

# targets -set -nocase -filter {name =~"RPU*"}
# clear_rpu_reset
# targets -set -nocase -filter {name =~"APU*"}
# loadhw -hw /home/kd/r5_qemu_workspace/r5_platform/export/r5_platform/hw/design_1_wrapper.xsa -mem-ranges [list {0x80000000 0xbfffffff} {0x400000000 0x5ffffffff} {0x1000000000 0x7fffffffff}] -regs
# configparams force-mem-access 1
# targets -set -nocase -filter {name =~ "*R5*#0"}
# rst -processor

# catch {XFsbl_TcmEccInit R5_L}
# dow $elf_file_path
# configparams force-mem-access 0