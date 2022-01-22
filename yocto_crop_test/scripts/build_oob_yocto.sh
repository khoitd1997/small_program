#!/bin/bash
# this script is used for building out of the box Yocto without
# any customization

set -e

# use working directory as where to build the software
build_dir="${PWD}"
yocto_release_url="http://downloads.yoctoproject.org/releases/yocto/yocto-3.3.3/poky-hardknott-25.0.3.tar.bz2"

echo "Starting Yocto Build"

wget "${yocto_release_url}" --output-document yocto_release.tar.bz2
rm -rf yocto_release
mkdir -p yocto_release
tar -xf yocto_release.tar.bz2 -C yocto_release --strip-components=1

cd yocto_release
source "./oe-init-build-env"

bitbake core-image-sato

echo "Yocto Build is Finished"