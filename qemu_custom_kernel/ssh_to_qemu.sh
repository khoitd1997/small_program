#!/bin/bash

set -e

sshpass -p root ssh root@localhost -p 5555 -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no