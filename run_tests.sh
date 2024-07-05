#!/usr/bin/env bash
set -e
df -h 

echo "Build IR of the C++ examples"
df -h
ninja -k 0 compile_cpp_examples
ls -al ./examples/cpp

echo "Build circuits(.crct) of the C++ examples"        
ninja -k 0  cpp_examples_generate_crct
ls -al ./examples/cpp

echo "Run simultaneous .tbl and .crct generation of the C++ examples"
df -h
ninja -k 0 cpp_examples_generate_both
ls -al ./examples/cpp

echo "Run size estimation for C++ examples"
df -h
ninja -k 0 cpp_examples_estimate_size

echo "Move examples' circuits and assignments for uploading"
df -h
chmod +x ../examples/move_artifacts.sh
bash ../examples/move_artifacts.sh examples/cpp ${ARTIFACTS_DIR}examples_output
ls -al ${ARTIFACTS_DIR}examples_output/

echo "Compile tests as cpp code"
df -h
ninja -k 0 all_tests_compile_as_cpp_code

echo "Compile tests as circuits"
df -h
ninja -k 0 all_tests_compile_as_circuits

echo "Run tests as cpp code (expected res calculation)"
df -h
ninja -k 0 all_tests_run_expected_res_calculation

echo "Get transpiler output for integration testing"
ninja -k 0 arithmetics_cpp_example_evm_verifier \
	  polynomial_cpp_example_evm_verifier \
	  poseidon_cpp_example_evm_verifier \
	  merkle_tree_poseidon_cpp_example_evm_verifier \
	  uint_remainder_cpp_evm_verifier \
	  compare_eq_cpp_evm_verifier \
	  private_input_cpp_evm_verifier

	  #uint_shift_left_evm_verifier \ TODO fix and re-enable tests
	  #uint_bit_decomposition_evm_verifier \ TODO fix and re-enable tests
	  #uint_bit_composition_evm_verifier \ TODO fix and re-enable tests

ls -al ./examples/cpp/
find -name "transpiler_output_*"
mkdir -p "$ARTIFACTS_DIR"cpp
mv examples/cpp/transpiler_output_* "$ARTIFACTS_DIR"cpp

echo "Run tests as circuits (real res calculation)"
df -h
ninja -k 0 all_tests_assign_circuits

echo "Compare expected and real test results"
df -h
cd ..
  # the following scripts are expected to run from the root of repository
  chmod +x ./tests/run_test_results_comparison.sh
  bash ./tests/run_test_results_comparison.sh

  echo "Run tests on faulty inputs"
  df -h
  chmod +x ./tests/run_tests_faulty_input.sh
  bash ./tests/run_tests_faulty_input.sh pallas
cd -

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

