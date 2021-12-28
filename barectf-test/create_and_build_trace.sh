#!/bin/bash

set -e

script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
config_dir="${script_dir}/configs"
config_file_path="${config_dir}/config.yml"

generated_kernel_meta_dir="${script_dir}/generated_kernel_trace_metadata"
generated_user_meta_dir="${script_dir}/generated_user_trace_metadata"
generated_src_dir="${script_dir}/barectf_generated_src"
build_dir="${script_dir}/build"
kernel_trace_dir="${build_dir}/kernel_trace"
user_trace_dir="${build_dir}/user_trace"
rm -rf "${generated_kernel_meta_dir}" \
       "${generated_user_meta_dir}" \
       "${generated_src_dir}" \
       "${kernel_trace_dir}" \
       "${user_trace_dir}"
mkdir "${generated_kernel_meta_dir}" \
      "${generated_user_meta_dir}" \
      "${generated_src_dir}" \
      "${kernel_trace_dir}" \
      "${user_trace_dir}"

cd "${generated_src_dir}"
cp "${config_file_path}" config_kernel.yml
sed -i 's/{{ENV_INCLUDE_FILE}}/env_kernel.yml/g' config_kernel.yml
barectf generate -I "${config_dir}" -m "${generated_kernel_meta_dir}" config_kernel.yml

cp "${config_file_path}" config_user.yml
sed -i 's/{{ENV_INCLUDE_FILE}}/env_user.yml/g' config_user.yml
barectf generate -I "${config_dir}" -m "${generated_user_meta_dir}" config_user.yml

mkdir -p "${build_dir}"
cd "${build_dir}"
cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build .

./test_barectf_app_exe lorem ipsum nulla dolore consequat

cp "${generated_kernel_meta_dir}/metadata" "${kernel_trace_dir}/metadata"

cp "${generated_user_meta_dir}/metadata" "${user_trace_dir}/metadata"
# event name for lttng/trace- compass needs to have colon but barectf doesn't work while trace name
# having colon in config.yml so need to do some mod to change the trace names to be correct
# after the metadata file has been generated
sed -i 's/lttng_ust_statedump_/lttng_ust_statedump:/g' "${user_trace_dir}/metadata"
sed -i 's/lttng_ust_libc_/lttng_ust_libc:/g' "${user_trace_dir}/metadata"

echo "Reading trace using babeltrace"
babeltrace2 user_trace