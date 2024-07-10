#!/usr/bin/env bash

          export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$(pwd)/libs/circifier/llvm/lib"
          echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/libs/circifier/llvm/lib"

          ninja -k 0 compile_rust_examples
          ls -al ./examples/rust/target/assigner-unknown-unknown/release/examples

          ninja -k 0 rust_examples_generate_crct
          ls -al ./examples/cpp

          ninja -k 0 rust_examples_generate_tbl
          ls -al ./examples/cpp

          ninja -k 0 prove_rust_examples

          ninja -k 0 compile_cpp_examples
          ls -al ./examples/cpp

          ninja -k 0 cpp_examples_generate_crct
          ls -al ./examples/cpp

          ninja -k 0 cpp_examples_generate_tbl_no_check
          ls -al ./examples/cpp

          bash ../tests/copy_assigner_outputs.sh ./examples/cpp/ separate_generatuon

          ninja -k 0 cpp_examples_generate_both
          ls -al ./examples/cpp

          bash ../tests/copy_assigner_outputs.sh ./examples/cpp/ simultaneous_generation

          bash ../tests/compare_folders_content.sh separate_generatuon simultaneous_generation

          ninja -k 0 cpp_examples_estimate_size

          chmod +x ./examples/copy_artifacts.sh
          ./examples/copy_artifacts.sh examples/cpp examples_output
          ls -al examples_output/

          ninja -k 0 all_tests_compile_as_cpp_code

          ninja -k 0 all_tests_compile_as_circuits

          ninja -k 0 all_tests_run_expected_res_calculation

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
          ninja -k 0  ${targets_str}
          ls -al examples/cpp/

          ninja -k 0 all_tests_assign_circuits

          chmod +x ./tests/run_test_results_comparison.sh
          bash ../tests/run_test_results_comparison.sh

          chmod +x ./tests/run_tests_faulty_input.sh
          bash ../tests/run_tests_faulty_input.sh pallas

          ninja -k 0 check-crypto3-assigner

          ninja -k 0 prove_cpp_examples

          ninja -k 0 recursive_gen

          ninja -k 0 compile_and_run_transpiler_tests

          ninja -k 0 recursion

