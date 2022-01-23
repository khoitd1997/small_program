#!/bin/bash

set -e

curr_script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
workdir="${curr_script_dir}/workdir"
scripts_dir="${curr_script_dir}/scripts"

# change between podman and Docker
# container_cmd="sudo docker"
container_cmd="podman"

rm -rf "${workdir}"
mkdir -p "${workdir}"

# the default image doesn't allow installing additional tool
# so we have to extend it a bit and then build our own
${container_cmd} build -t yocto_build_container - < "${curr_script_dir}/Dockerfile"

${container_cmd} run --rm -it \
    -v "${workdir}":/workdir \
    -v "${scripts_dir}":/scripts \
    --workdir=/workdir \
    yocto_build_container "/scripts/build_yocto.sh"