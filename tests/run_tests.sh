#!/bin/bash

color_red() { echo -e "\033[31m$1\033[0m"; }
color_green() { echo -e "\033[32m$1\033[0m"; }

get_script_dir() {
    echo "$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
}

curve_type=$1

test_example_with_file() {
    local test_example=$1
    local file=$2
    local filename=$(basename -- "$file")
    filename="${filename%.inp}"

    local output_dir="./assigner_results/${test_example}/${filename}"
    local log_dir="./assigner_real_logs/${test_example}"
    local expected_log="./assigner_expected_logs/${test_example}/${filename}.log"
    local real_log="./assigner_real_logs/${test_example}/${filename}.log"

    echo -n "  ${filename}: "

    # Execute and Log
    mkdir -p "$output_dir" "$log_dir"
    local assigner_flags="--max-num-provers 2"
    [[ $test_example != *"multi_provers"* ]] && assigner_flags=""

    if ! "${script_dir}/../build/bin/assigner/assigner" -b "${script_dir}/../build/tests/cpp/${test_example////_}.ll" -i "$file" -t "${output_dir}/assignment.tbl" -c "${output_dir}/circuit.crct" -e $curve_type  --check --print_circuit_output $assigner_flags > "${log_dir}/${filename}.log"; then
        color_red "Assigner failed"
        return 1

    fi

    # Compare Logs
    mkdir -p "$(dirname "$expected_log")"
    if ! "${script_dir}/../build/tests/cpp/${test_example////_}_calculate_expected_res" "$file" > "$expected_log"; then
        color_red "Expected result calculation failed"
        return 1
    fi

    if ! diff -u "$expected_log" "$real_log" > diff.log; then
        color_red "Comparison failed"
        echo "Expected:"
        cat $expected_log
        echo "Real:"
        cat $real_log
        echo "Diff output:"
        cat diff.log
        return 1
    fi

    color_green "Success"
}

script_dir=$(get_script_dir)
at_least_one_test_launched=0
exit_code=0
clean_up=0

for arg in "$@"; do
    if [[ $arg == "--clean" ]]; then
        clean_up=1
    fi
done

while IFS= read -r test_example || [[ -n "$test_example" ]]; do
    echo "Testing ${test_example} with inputs:"
    for file in "$script_dir/inputs/$(dirname "$test_example")"/*.inp; do
        if ! test_example_with_file "$test_example" "$file"; then
            exit_code=1
        fi
        at_least_one_test_launched=1
    done
    echo
done < "$script_dir/tests_list_$curve_type.txt"

if [[ $clean_up -eq 1 ]]; then
    echo "Cleaning up generated directories..."
    rm -rf ./assigner_results ./assigner_real_logs ./assigner_expected_logs
fi

if [ $at_least_one_test_launched -eq 0 ]; then
    echo "No tests were launched."
    exit 1
fi

exit $exit_code
