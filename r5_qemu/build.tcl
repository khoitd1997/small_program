setws ./r5_workspace
platform create -name HW1 -hw ./zcu102.xsa -no-boot-bsp
# to get the list of available options, just enter some nonsense name and the error message should
# show the possible options
# valid OS are "freertos10_xilinx freertos standalone linux"
domain create -name r5_freertos -os freertos10_xilinx -proc psu_cortexr5_0
domain active r5_freertos

# Availabel libraries are : "libmetal lwip211 openamp xilrsa xilpm xilfpga xilnvm xilffs xilisf xilflash xilpuf xilskey xilsecure"
bsp setlib -name libmetal
bsp setlib -name lwip211
bsp setlib -name openamp

platform generate
# app create -name r5_test_app -platform HW1 -template "Zynq MP FSBL" -domain r5_freertos -lang skdafjlasdjfkla
# app build -name r5_test_app

