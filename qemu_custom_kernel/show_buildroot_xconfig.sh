#!/bin/bash

set -e

script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
source "${script_dir}/common.sh"

cd "${buildroot_dir}"

make xconfig BR2_DEFCONFIG="${buildroot_defconfig_path}"

cd "${script_dir}" 