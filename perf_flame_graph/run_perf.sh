#!/bin/bash

set -e

script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
workdir_dir="${script_dir}/workdir"
flamegraph_dir="${workdir_dir}/FlameGraph"

mkdir -p "${workdir_dir}"
cd "${workdir_dir}"

if [ ! -e "${flamegraph_dir}" ]; then
    git clone https://github.com/brendangregg/FlameGraph.git
fi

sudo perf record --user-callchains -F 99 -a -g -- sleep 2

sudo chown "${USER}" perf.data
perf script > out.perf

"${flamegraph_dir}/stackcollapse-perf.pl" --all ./out.perf | "${flamegraph_dir}/flamegraph.pl" --hash > "${script_dir}"/perf_graph/example-perf.svg

# open a separate window so we can use that one for live server
code "${script_dir}"

# firefox ./example-perf.svg &