#!/bin/bash

set -e

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

session_name="kernel_trace_session"

set +e
lttng destroy ${session_name}
set -e

rm -rf "${script_dir}/trace_output"
lttng create ${session_name} --output="${script_dir}/trace_output"

# lttng enable-channel --kernel kernel_channel --num-subbuf=4 --subbuf-size=15M

# syscall from: lttng list --kernel --syscall
# lttng enable-event --kernel -c kernel_channel --syscall futex
# lttng enable-event --kernel -c kernel_channel -a

# lttng enable-event --kernel -c kernel_channel sched_*
# lttng enable-event --kernel -c kernel_channel sched_switch,sched_wakeup,sys_exit_futex,sys_enter_futex

lttng enable-channel --userspace user_channel --num-subbuf=4 --subbuf-size=15M
lttng enable-event -u -c user_channel -a
lttng add-context -u -c user_channel -t vpid -t vtid -t procname

# liblttng-ust-libc-wrapper
# liblttng-ust-pthread-wrapper
# liblttng-ust-cyg-profile
# liblttng-ust-cyg-profile-fast
# liblttng-ust-dl

lttng start ${session_name}

set +e
LD_PRELOAD="liblttng-ust-cyg-profile.so:liblttng-ust-libc-wrapper.so" "${script_dir}/test_cpp_app/build/test_cmake_app_exe"
set -e
# sleep 10
lttng destroy ${session_name}
chown -R "${SUDO_USER}" "${script_dir}/trace_output"

cat /proc/kallsyms > "${script_dir}/kernel_symbols.txt"