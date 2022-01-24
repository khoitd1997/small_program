#!/bin/bash

set -e

if [ "$EUID" -ne 0 ]; then 
    echo "This script must be run as root!"
    exit 1
fi


curr_script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
workdir="${curr_script_dir}/workdir_docker"
scripts_dir="${curr_script_dir}/scripts"

run_infinite_test="true"

while :
do
    rm -rf "${workdir}"
    mkdir -p "${workdir}"
    chown -R "${SUDO_USER}": "${workdir}"

    # the default image doesn't allow installing additional tool
    # so we have to extend it a bit and then build our own
    docker build -t yocto_build_container - < "${curr_script_dir}/docker.dockerfile"

    docker run --rm -it \
        -v "${workdir}":/workdir \
        -v "${scripts_dir}":/scripts \
        --workdir=/workdir \
        yocto_build_container "/scripts/build_yocto.sh"
        
    if [ "$run_infinite_test" != "true" ]; then
        break
    fi

done