#!/bin/bash
# this script is used to test the stability of using Docker
# by rerunning the build over and over

set -e

curr_script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
counter=0

while :
do
    echo "On test iteration number ${counter}"
	bash "${curr_script_dir}/run_podman.sh"
    (( counter++ ))
done