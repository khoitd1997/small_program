#!/bin/bash
# NOTE: This doesn't work very well at least for R5 so probably don't use it
# mostly here for references

set -e

script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

exe_path="${script_dir}/build/r5_test.elf"
cd "${script_dir}/build"
gdb-multiarch -x "${script_dir}/.gdbinit"