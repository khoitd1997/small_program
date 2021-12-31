#!/bin/bash

set -e

# d630dfedc5b873eac48e734f97c995ff393c1067 is the hash of the commit that
# the sw repo was first checked in
git diff d630dfedc5b873eac48e734f97c995ff393c1067 r5_qemu/my_vitis_repo/sw_servcices/my_lwip211 > r5_qemu/change_list.patch