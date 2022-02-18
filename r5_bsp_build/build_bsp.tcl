set xsa_file_path [lindex $argv 0]
set custom_repo_path [lindex $argv 1]

# always wipe the workspace to be sure we are starting fresh
# the logic to smartly rebuild should be already be in the cmake files
file delete -force ./r5_workspace

setws ./r5_workspace
platform create -name r5_platform -hw "$xsa_file_path" -no-boot-bsp

# NOTE: have to add custom repo early to make sure domain create use our copy of freeRTOS
hsi::set_repo_path "$custom_repo_path"
# some weird things happen sometimes if we don't add a short delay after
# setting the repo path
after 1000

# to get the list of available options, just enter some nonsense name and the error message should
# show the possible options
# valid OS are "freertos10_xilinx freertos standalone linux"
domain create -name r5_freertos -os freertos10_xilinx -proc psu_cortexr5_0
domain active r5_freertos

# Availabel libraries are : "libmetal lwip211 openamp xilrsa xilpm xilfpga xilnvm xilffs xilisf xilflash xilpuf xilskey xilsecure"
bsp setlib -name libmetal
bsp setlib -name my_lwip211
bsp setlib -name openamp

platform generate

