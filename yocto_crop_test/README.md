# Yocto Crop Test

For testing the [Crop container](https://github.com/crops/poky-container) that can be used for Yocto CI build. This folder also includes scripts to run 2 vagrant container that either has Vitis or doesn't

## Setup

Depending on what needs to be run, you might need `vagrant`, `docker`, `podman`

```shell
# install required vagrant plugins
vagrant plugin install vagrant-disksize

# for Vagrant
sudo apt install virtualbox -y
```

## Usage

To run builds:

```shell
cd <path-to-folder-of-this-README>

./run_docker.sh

./run_podman.sh

# this is fairly standalone and doesn't require a lot of provisions
./run_vagrant_without_vitis.sh

# this require having Vitis installation files in vagrant_mount folder
# check Vagrantfile to see what files are expected
./run_vagrant_with_vitis.sh
```

## Notes

Docker builds fine as long as the yocto-downloaded xsct is used instead of using the one installed on the host and exposed through volume
