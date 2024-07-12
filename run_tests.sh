#!/usr/bin/env bash

df -h 

echo "Build IR of the C++ examples"
df -h
ninja -k 0 compile_cpp_examples
ls -al ./examples/cpp

echo "Build circuits(.crct) of the C++ examples"
df -h
ninja -k 0 cpp_examples_generate_crct
ls -al ./examples/cpp

echo "Build assignment tables(.tbl) of the C++ examples"
df -h
ninja -k 0 cpp_examples_generate_tbl_no_check
ls -al ./examples/cpp

echo "Copy assigner output from separate generation mode"
df -h
bash ../tests/copy_assigner_outputs.sh ./examples/cpp/ separate_generatuon

echo "Run simultaneous .tbl and .crct generation of the C++ examples"
df -h
ninja -k 0 cpp_examples_generate_both
ls -al ./examples/cpp

echo "Copy assigner output from simultaneous generation mode"
df -h
bash ../tests/copy_assigner_outputs.sh ./examples/cpp/ simultaneous_generation

echo "Compare different assigner modes output"
df -h
bash ../tests/compare_folders_content.sh separate_generatuon simultaneous_generation

echo "Run size estimation for C++ examples"
df -h
ninja -k 0 cpp_examples_estimate_size

echo "Copy examples' circuits and assignments for uploading"
df -h
chmod +x ./examples/copy_artifacts.sh
bash ../examples/copy_artifacts.sh examples/cpp examples_output
ls -al examples_output/

echo "Compile tests as cpp code"
df -h
ninja -k 0 all_tests_compile_as_cpp_code -j8

echo "Compile tests as circuits"
df -h
ninja -k 0 all_tests_compile_as_circuits -j8

echo "Run tests as cpp code (expected res calculation)"
df -h
ninja -k 0 all_tests_run_expected_res_calculation

echo "Get transpiler output for integration testing"
INTEGRATION_TESTING_TARGETS=(
	  "arithmetics_cpp_example_evm_verifier"
	  "polynomial_cpp_example_evm_verifier"
	  "poseidon_cpp_example_evm_verifier"
	  "merkle_tree_poseidon_cpp_example_evm_verifier"
	  "uint_remainder_cpp_evm_verifier"
	  "uint_shift_left_evm_verifier"
	  "uint_bit_decomposition_evm_verifier"
	  "uint_bit_composition_evm_verifier"
	  "compare_eq_cpp_evm_verifier"
	  "private_input_cpp_evm_verifier"
)

targets_str=$(echo "${INTEGRATION_TESTING_TARGETS}" | awk 'NF {printf " %s_evm_verifier ", $0}' | sed 's/[[:space:]]*$//')
echo "targets: ${targets_str}"
df -h
ninja -k 0  ${targets_str}
ls -al examples/cpp/

echo "Run tests as circuits (real res calculation)"
df -h
ninja -k 0 all_tests_assign_circuits

echo "Compare expected and real test results"
df -h
chmod +x ./tests/run_test_results_comparison.sh
bash ../tests/run_test_results_comparison.sh

echo "Run tests on faulty inputs"
df -h
chmod +x ./tests/run_tests_faulty_input.sh
bash ../tests/run_tests_faulty_input.sh pallas

echo "Run assigner unit tests"
df -h
ninja -k 0 check-crypto3-assigner

echo "Build proof for the circuit of the C++ examples"
df -h
ninja -k 0 prove_cpp_examples

echo "Build recursive gen"
df -h
ninja -k 0 recursive_gen

echo "Build and run transpiler test"
df -h
ninja -k 0 compile_and_run_transpiler_tests

echo "Run recursive verifier tests"
df -h
ninja -k 0 recursion

echo "finish"

