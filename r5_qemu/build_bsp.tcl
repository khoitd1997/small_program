set xsa_file_path [lindex $argv 0]
set custom_repo_path [lindex $argv 1]

setws ./r5_workspace
platform create -name r5_platform -hw "$xsa_file_path" -no-boot-bsp
# to get the list of available options, just enter some nonsense name and the error message should
# show the possible options
# valid OS are "freertos10_xilinx freertos standalone linux"
domain create -name r5_freertos -os freertos10_xilinx -proc psu_cortexr5_0
domain active r5_freertos

hsi::set_repo_path "$custom_repo_path"

# Availabel libraries are : "libmetal lwip211 openamp xilrsa xilpm xilfpga xilnvm xilffs xilisf xilflash xilpuf xilskey xilsecure"
bsp setlib -name libmetal
bsp setlib -name my_lwip211
bsp setlib -name openamp


platform generate
# app create -name r5_test_app -platform HW1 -template "Zynq MP FSBL" -domain r5_freertos -lang skdafjlasdjfkla
# app build -name r5_test_app

