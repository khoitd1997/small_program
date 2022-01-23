#!/bin/bash

curr_script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
vagrant_name="with_vitis"

set -e

cd "${curr_script_dir}"

# vagrant destroy -f
vagrant up --provider=virtualbox "${vagrant_name}"
vagrant ssh "${vagrant_name}"