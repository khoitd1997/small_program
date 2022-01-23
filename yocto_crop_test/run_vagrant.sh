#!/bin/bash
# script to build a Vagrant image for building Yocto
# used to check if issue is Docker specific or not

curr_script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

set -e

cd "${curr_script_dir}"

# vagrant destroy -f
vagrant up --provider=virtualbox
vagrant ssh