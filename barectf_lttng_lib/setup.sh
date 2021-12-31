#!/bin/bash

set -e

# get from https://babeltrace.org/
sudo apt-add-repository ppa:lttng/ppa
sudo apt-get update
sudo apt-get install babeltrace2

# the python3-barectf package seems outdated, install from pip to be sure
sudo pip3 install barectf