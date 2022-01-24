#!/bin/bash

set -e

curr_script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
workdir="${curr_script_dir}/workdir_podman"
scripts_dir="${curr_script_dir}/scripts"

rm -rf "${workdir}"
mkdir -p "${workdir}"

podman build -t yocto_podman_build_container - < "${curr_script_dir}/podman.dockerfile"

podman run --rm -it \
    -v "${workdir}":/workdir \
    -v "${scripts_dir}":/scripts \
    -w "/workdir" \
    yocto_podman_build_container "/scripts/build_yocto.sh"