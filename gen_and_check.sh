set -e
#${ZKLLVM_ninja:-ninja}/bin/recursive_gen/recursive_gen -h
ninja -C ${ZKLLVM_BUILD:-build} assigner transpiler recursive_gen -j4

#${ZKLLVM_ninja:-build}/bin/recursive_gen/recursive_gen -m gen-verifier -t ./build/examples/cpp/assignment_pallas_curve_multiplication_cpp_example.tbl -c ./build/examples/cpp/circuit_pallas_curve_multiplication_cpp_example.crct -o ./recursive_input
#${ZKLLVM_ninja:-build}/bin/recursive_gen/recursive_gen -m gen-input -t ./build/examples/cpp/assignment_pallas_curve_multiplication_cpp_example.tbl -c ./build/examples/cpp/circuit_pallas_curve_multiplication_cpp_example.crct -o ./recursive_input

#${ZKLLVM_ninja:-build}/bin/recursive_gen/recursive_gen -m gen-verifier -t ./build/examples/cpp/assignment_arithmetics_cpp_example.tbl -c ./build/examples/cpp/circuit_arithmetics_cpp_example.crct -o ./recursive_input
#${ZKLLVM_ninja:-build}/bin/recursive_gen/recursive_gen -m gen-input -t ./build/examples/cpp/assignment_arithmetics_cpp_example.tbl -c ./build/examples/cpp/circuit_arithmetics_cpp_example.crct -o ./recursive_input

#${ZKLLVM_ninja:-build}/bin/recursive_gen/recursive_gen -m gen-verifier -t ./build/examples/cpp/assignment_integer_arithmetics_cpp_example.tbl -c ./build/examples/cpp/circuit_integer_arithmetics_cpp_example.crct -o ./recursive_input
#${ZKLLVM_ninja:-build}/bin/recursive_gen/recursive_gen -m gen-input -t ./build/examples/cpp/assignment_integer_arithmetics_cpp_example.tbl -c ./build/examples/cpp/circuit_integer_arithmetics_cpp_example.crct -o ./recursive_input

#${ZKLLVM_ninja:-build}/bin/recursive_gen/recursive_gen -m gen-verifier -t ./build/examples/cpp/assignment_balance_cpp_example.tbl -c ./build/examples/cpp/circuit_balance_cpp_example.crct -o ./recursive_input
#${ZKLLVM_ninja:-build}/bin/recursive_gen/recursive_gen -m gen-input -t ./build/examples/cpp/assignment_balance_cpp_example.tbl -c ./build/examples/cpp/circuit_balance_cpp_example.crct -o ./recursive_input

${ZKLLVM_ninja:-build}/bin/recursive_gen/recursive_gen -m gen-verifier -t ./build/examples/cpp/assignment_merkle_tree_poseidon_cpp_example.tbl -c ./build/examples/cpp/circuit_merkle_tree_poseidon_cpp_example.crct -o ./recursive_input
${ZKLLVM_ninja:-build}/bin/recursive_gen/recursive_gen -m gen-input -t ./build/examples/cpp/assignment_merkle_tree_poseidon_cpp_example.tbl -c ./build/examples/cpp/circuit_merkle_tree_poseidon_cpp_example.crct -o ./recursive_input

#${ZKLLVM_ninja:-build}/bin/recursive_gen/recursive_gen -m gen-verifier -t ./build/examples/cpp/assignment_poseidon_cpp_example.tbl -c ./build/examples/cpp/circuit_poseidon_cpp_example.crct -o ./recursive_input
#${ZKLLVM_ninja:-build}/bin/recursive_gen/recursive_gen -m gen-input -t ./build/examples/cpp/assignment_poseidon_cpp_example.tbl -c ./build/examples/cpp/circuit_poseidon_cpp_example.crct -o ./recursive_input

#${ZKLLVM_ninja:-build}/bin/recursive_gen/recursive_gen -m gen-verifier -t ./build/examples/cpp/assignment_merkle_tree_sha2_256_cpp_example.tbl -c ./build/examples/cpp/circuit_merkle_tree_sha2_256_cpp_example.crct -o ./recursive_input
#${ZKLLVM_ninja:-build}/bin/recursive_gen/recursive_gen -m gen-input -t ./build/examples/cpp/assignment_merkle_tree_sha2_256_cpp_example.tbl -c ./build/examples/cpp/circuit_merkle_tree_sha2_256_cpp_example.crct -o ./recursive_input

#${ZKLLVM_ninja:-build}/bin/recursive_gen/recursive_gen -m gen-verifier -t ./build/examples/cpp/assignment_memory_cpp_example.tbl -c ./build/examples/cpp/circuit_memory_cpp_example.crct -o ./recursive_input
#${ZKLLVM_ninja:-build}/bin/recursive_gen/recursive_gen -m gen-input -t ./build/examples/cpp/assignment_memory_cpp_example.tbl -c ./build/examples/cpp/circuit_memory_cpp_example.crct -o ./recursive_input

cp -rf ./recursive_input/placeholder_verifier.cpp ./examples/cpp/placeholder_verifier.cpp
cp -rf ./recursive_input/placeholder_verifier.inp ./examples/inputs/placeholder_verifier.inp
#cp -rf ./recursive/placeholder_verifier_0.cpp ./examples/cpp/placeholder_verifier.cpp
#cp -rf ./recursive/placeholder_verifier_0.inp ./examples/inputs/placeholder_verifier.inp

ninja -C ${ZKLLVM_BUILD:-build} placeholder_verifier_cpp_example_assign -j4
#${ZKLLVM_ninja:-build}/bin/transpiler/transpiler -m gen-test-proof -t ./build/examples/cpp/assignment_placeholder_verifier_cpp_example.tbl -c ./build/examples/cpp/circuit_placeholder_verifier_cpp_example.crct -o ./build/examples/cpp/transpiler_output_placeholder_verifier_cpp_example
#${ZKLLVM_ninja:-build}/bin/transpiler/transpiler -m gen-evm-verifier -t ./build/examples/cpp/assignment_placeholder_verifier_cpp_example.tbl -c ./build/examples/cpp/circuit_placeholder_verifier_cpp_example.crct -o ./build/examples/cpp/transpiler_output_placeholder_verifier_cpp_example