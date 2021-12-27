#!/bin/bash

set -e

script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

session_name="kernel_trace_session"

set +e
lttng destroy ${session_name}
set -e

rm -rf "${script_dir}/trace_output"
lttng create ${session_name} --output="${script_dir}/trace_output"
lttng enable-channel --kernel kernel_channel --num-subbuf=4 --subbuf-size=15M
# lttng enable-event --kernel -c kernel_channel sched_*
lttng enable-event --kernel -c kernel_channel sched_switch,sched_wakeup,lock*

lttng start ${session_name}

# liblttng-ust-libc-wrapper
# liblttng-ust-pthread-wrapper
# liblttng-ust-cyg-profile
# liblttng-ust-cyg-profile-fast
# liblttng-ust-dl

sleep 10
lttng destroy ${session_name}

cat /proc/kallsyms > "${script_dir}/kernel_symbols.txt"