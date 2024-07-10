#!/usr/bin/env bash

TESTS=(
	  "compile_cpp_examples"
	  "cpp_examples_generate_crct"
	  "cpp_examples_generate_tbl_no_check"
	  "cpp_examples_generate_both"
	  "cpp_examples_estimate_size"
	  "all_tests_compile_as_cpp_code"
	  "all_tests_compile_as_circuits"
	  "all_tests_run_expected_res_calculation"
	  "all_tests_assign_circuits"
	  "check-crypto3-assigner"
	  "prove_cpp_examples"
	  "recursive_gen"
	  "compile_and_run_transpiler_tests"
	  "recursion"
	  # Integration tests
	  "arithmetics_cpp_example"
	  "polynomial_cpp_example"
	  "poseidon_cpp_example"
	  "merkle_tree_poseidon_cpp_example"
	  "uint_remainder_cpp"
	  "uint_shift_left"
	  "uint_bit_decomposition"
	  "uint_bit_composition"
	  "compare_eq_cpp"
	  "private_input_cpp"
)

ninja -k0 "${TESTS[@]}"
