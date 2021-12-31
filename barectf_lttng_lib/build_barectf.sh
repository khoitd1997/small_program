#!/bin/bash

set -e

while [ "$#" -ne 0 ]; do
    case "$1" in
        --config-dir)
            config_dir="${2}"
            config_file_path="${config_dir}/config.yml"
            shift
            ;;
        --build-dir)
            build_dir="${2}"
            shift
            ;;
        --trace-root-dir)
            trace_root_dir="${2}"
            shift
            ;;
        *)
            echo "Unknown arg: ${1}"
            exit 1
    esac
    shift
done

generated_kernel_meta_dir="${build_dir}/generated_kernel_trace_metadata"
generated_user_meta_dir="${build_dir}/generated_user_trace_metadata"
generated_src_dir="${build_dir}/barectf_generated_src"
kernel_trace_dir="${trace_root_dir}/kernel_trace"
user_trace_dir="${trace_root_dir}/user_trace"

rm -rf "${generated_kernel_meta_dir}" \
       "${generated_user_meta_dir}" \
       "${generated_src_dir}"
mkdir -p "${generated_kernel_meta_dir}" \
      "${generated_user_meta_dir}" \
      "${generated_src_dir}" \
      "${kernel_trace_dir}" \
      "${user_trace_dir}"

cd "${generated_src_dir}"
cp "${config_file_path}" config_kernel_temp.yml
sed -i 's/{{ENV_INCLUDE_FILE}}/env_kernel.yml/g' config_kernel_temp.yml
barectf generate -I "${config_dir}" -m "${generated_kernel_meta_dir}" config_kernel_temp.yml
cp "${generated_kernel_meta_dir}/metadata" "${kernel_trace_dir}/metadata"

cp "${config_file_path}" config_user_temp.yml
sed -i 's/{{ENV_INCLUDE_FILE}}/env_user.yml/g' config_user_temp.yml
barectf generate -I "${config_dir}" -m "${generated_user_meta_dir}" config_user_temp.yml
cp "${generated_user_meta_dir}/metadata" "${user_trace_dir}/metadata"

# event name for lttng/trace- compass needs to have colon but barectf doesn't work while trace name
# having colon in config.yml so need to do some mod to change the trace names to be correct
# after the metadata file has been generated
sed -i 's/lttng_ust_statedump_/lttng_ust_statedump:/g' "${user_trace_dir}/metadata"
sed -i 's/lttng_ust_libc_/lttng_ust_libc:/g' "${user_trace_dir}/metadata"
sed -i 's/lttng_ust_cyg_profile_/lttng_ust_cyg_profile:/g' "${user_trace_dir}/metadata"