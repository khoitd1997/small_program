#!/bin/bash

set -e

script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

session_name="kernel_trace_session"

set +e
sudo lttng destroy ${session_name}
set -e

sudo rm -rf ${script_dir}/trace_output
sudo lttng create ${session_name} --output=${script_dir}/trace_output
sudo lttng enable-channel --kernel kernel_channel --num-subbuf=4 --subbuf-size=15M
sudo lttng enable-event --kernel -c kernel_channel -a
sudo lttng add-context -k -c kernel_channel -t callstack-kernel
sudo lttng add-context -k -c kernel_channel -t callstack-user

sudo lttng start ${session_name}

# liblttng-ust-libc-wrapper
# liblttng-ust-pthread-wrapper
# liblttng-ust-cyg-profile
# liblttng-ust-cyg-profile-fast
# liblttng-ust-dl

LD_PRELOAD="liblttng-ust-cyg-profile-fast.so" ${script_dir}/test_cpp_app/build/test_cmake_app_exe
sudo lttng destroy ${session_name}
sudo chown -R ${USER} ${script_dir}/trace_output

sudo cat /proc/kallsyms > ${script_dir}/kernel_symbols.txt