#!/bin/bash

set -e

script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

session_name="kernel_trace_session"

set +e
lttng destroy ${session_name}
set -e

sudo rm -rf ${script_dir}/trace_output
sudo lttng create ${session_name} --output=${script_dir}/trace_output
sudo lttng enable-event --kernel sched_switch,sched_process_fork

sudo lttng start ${session_name}
sleep 2
sudo lttng destroy ${session_name}
sudo chown -R ${USER} ${script_dir}/trace_output