#!/bin/bash

usage() {
    echo "Usage: $0 <base-directory> <target-directory>"
    exit 1
}

if [ "$#" -ne 2 ]; then
    usage
fi

base_dir=$1
target_dir=$(realpath "$2")

if [ ! -d "$base_dir" ]; then
    echo "Error: Base directory '$base_dir' does not exist."
    exit 1
fi

mkdir -p "$target_dir"

cd "$base_dir" || exit

declare -A base_names
for ll_file in *.ll; do
    base_name=$(echo "$ll_file" | sed -E 's/_no_stdlib.*//' | sed -E 's/\.ll$//')
    base_names["$base_name"]=1
done

for base_name in "${!base_names[@]}"; do
    echo "Processing $base_name"

    mkdir -p "$target_dir/$base_name"

    for variant in "$base_name"*".ll"; do
        if [[ -f "$variant" ]]; then
            suffix=$(echo "$variant" | sed -E "s/${base_name}(_no_stdlib.*)?\.ll/\1/")
            suffix=${suffix:-""}
            mv "$variant" "$target_dir/$base_name/bytecode${suffix}.ll"
        fi
    done

    if [[ -f "circuit_${base_name}.crct" ]]; then
        mv "circuit_${base_name}.crct" "$target_dir/$base_name/circuit.crct"
    fi
    if [[ -f "assignment_${base_name}.tbl" ]]; then
        mv "assignment_${base_name}.tbl" "$target_dir/$base_name/assignment.tbl"
    fi
done

echo "Files have been moved from $base_dir to $target_dir"
