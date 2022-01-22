#!/bin/bash

set -e

curr_script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
workdir="${curr_script_dir}/workdir"
scripts_dir="${curr_script_dir}/scripts"

rm -rf "${workdir}"
mkdir -p "${workdir}"

# the default image doesn't allow installing additional tool
# so we have to extend it a bit and then build our own
sudo docker build -t yocto_build_container .

sudo docker run --rm -it \
    -v "${workdir}":/workdir \
    -v "${scripts_dir}":/scripts \
    -v /tools:/tools \
    --workdir=/workdir \
    yocto_build_container "/scripts/build_yocto.sh"