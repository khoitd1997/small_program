#!/bin/bash
# usage: vscode-ssh <path-on-server>
# example: vscode-ssh "${HOME}"

set -e
echo "Path: ${1}"

code --folder-uri "vscode-remote://ssh-remote+root@localhost:5555/${1}"