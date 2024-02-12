set -e
mkdir -p ./recursive
#ninja -C build arithmetics_cpp_example_assign -j10
#./build/bin/recursive_gen/recursive_gen -m gen-input -i ./examples/inputs/arithmetics.inp -t ./build/examples/cpp/assignment_arithmetics_cpp_example.tbl -c ./build/examples/cpp/circuit_arithmetics_cpp_example.crct -o ./recursive -e pallas -p 2
#./build/bin/recursive_gen/recursive_gen -m gen-verifier -i ./examples/inputs/arithmetics.inp -t ./build/examples/cpp/assignment_arithmetics_cpp_example.tbl -c ./build/examples/cpp/circuit_arithmetics_cpp_example.crct -o ./recursive -e pallas -p 2
ninja -C build merkle_tree_poseidon_cpp_example_assign
./build/bin/recursive_gen/recursive_gen -m gen-input -i ./examples/inputs/merkle_tree_poseidon.inp -t ./build/examples/cpp/assignment_merkle_tree_poseidon_cpp_example.tbl -c ./build/examples/cpp/circuit_merkle_tree_poseidon_cpp_example.crct -o ./recursive -e pallas
./build/bin/recursive_gen/recursive_gen -m gen-verifier -i ./examples/inputs/merkle_tree_poseidon.inp -t ./build/examples/cpp/assignment_merkle_tree_poseidon_cpp_example.tbl -c ./build/examples/cpp/circuit_merkle_tree_poseidon_cpp_example.crct -o ./recursive -e pallas
#ninja -C build integer_arithmetics_cpp_example_assign
#./build/bin/recursive_gen/recursive_gen -m gen-input -i ./examples/inputs/integer_arithmetics.inp -t ./build/examples/cpp/assignment_integer_arithmetics_cpp_example.tbl -c ./build/examples/cpp/circuit_integer_arithmetics_cpp_example.crct -o ./recursive -e pallas
#./build/bin/recursive_gen/recursive_gen -m gen-verifier -i ./examples/inputs/integer_arithmetics.inp -t ./build/examples/cpp/assignment_integer_arithmetics_cpp_example.tbl -c ./build/examples/cpp/circuit_integer_arithmetics_cpp_example.crct -o ./recursive -e pallas
#ninja -C build balance_cpp_example_assign
#./build/bin/recursive_gen/recursive_gen -m gen-input -i ./examples/inputs/balance.inp -t ./build/examples/cpp/assignment_balance_cpp_example.tbl -c ./build/examples/cpp/circuit_balance_cpp_example.crct -o ./recursive -e pallas
#./build/bin/recursive_gen/recursive_gen -m gen-verifier -i ./examples/inputs/balance.inp -t ./build/examples/cpp/assignment_balance_cpp_example.tbl -c ./build/examples/cpp/circuit_balance_cpp_example.crct -o ./recursive -e pallas
#ninja -C build poseidon_cpp_example_assign
#./build/bin/recursive_gen/recursive_gen -m gen-input -i ./examples/inputs/poseidon.inp -t ./build/examples/cpp/assignment_poseidon_cpp_example.tbl -c ./build/examples/cpp/circuit_poseidon_cpp_example.crct -o ./recursive -e pallas
#./build/bin/recursive_gen/recursive_gen -m gen-verifier -i ./examples/inputs/poseidon.inp -t ./build/examples/cpp/assignment_poseidon_cpp_example.tbl -c ./build/examples/cpp/circuit_poseidon_cpp_example.crct -o ./recursive -e pallas
#ninja -C build pallas_curve_addition_cpp_example_assign
#./build/bin/recursive_gen/recursive_gen -m gen-input -i ./examples/inputs/pallas_curve_examples/pallas_curve_add.inp -t ./build/examples/cpp/assignment_pallas_curve_addition_cpp_example.tbl -c ./build/examples/cpp/circuit_pallas_curve_addition_cpp_example.crct -o ./recursive -e pallas
#./build/bin/recursive_gen/recursive_gen -m gen-verifier -i ./examples/inputs/pallas_curve_examples/pallas_curve_add.inp -t ./build/examples/cpp/assignment_pallas_curve_addition_cpp_example.tbl -c ./build/examples/cpp/circuit_pallas_curve_addition_cpp_example.crct -o ./recursive -e pallas
#ninja -C build polynomial_cpp_example_assign
#./build/bin/recursive_gen/recursive_gen -m gen-input -i ./examples/inputs/polynomial.inp -t ./build/examples/cpp/assignment_polynomial_cpp_example.tbl -c ./build/examples/cpp/circuit_polynomial_cpp_example.crct -o ./recursive -e pallas
#./build/bin/recursive_gen/recursive_gen -m gen-verifier -i ./examples/inputs/polynomial.inp -t ./build/examples/cpp/assignment_polynomial_cpp_example.tbl -c ./build/examples/cpp/circuit_polynomial_cpp_example.crct -o ./recursive -e pallas

cp -rf ./recursive/placeholder_verifier.cpp ./examples/cpp/placeholder_verifier.cpp
cp -rf ./recursive/placeholder_verifier.inp ./examples/inputs/placeholder_verifier.inp

ninja -C build placeholder_verifier_cpp_example
./build_recursive/bin/assigner/assigner -i ./examples/inputs/placeholder_verifier.inp -b ./build/examples/cpp/placeholder_verifier_cpp_example.ll -c recursive/circuit.crct -t recursive/assignment.tbl --max-num-provers 100 -e pallas --check

## It is second recursion layer. TODO: run on the server

#mkdir -p ./recursive2
#./build_recursive/bin/recursive_gen/recursive_gen -m gen-input -i ./examples/inputs/placeholder_verifier.inp -t ./recursive/assignment.tbl -c ./recursive/circuit.crct -o ./recursive2 -e pallas
#./build_recursive/bin/recursive_gen/recursive_gen -m gen-verifier -i ./examples/inputs/placeholder_verifier.inp -t ./recursive/assignment.tbl -c ./recursive/circuit.crct -o ./recursive2 -e pallas
#cp -rf ./recursive2/placeholder_verifier.cpp ./examples/cpp/placeholder_verifier.cpp
#cp -rf ./recursive2/placeholder_verifier.inp ./examples/inputs/placeholder_verifier.inp
#ninja -C build_recursive placeholder_verifier_cpp_example
#./build_recursive/bin/assigner/assigner -i ./examples/inputs/placeholder_verifier.inp -b ./build/examples/cpp/placeholder_verifier_cpp_example.ll -c recursive2/circuit.crct -t recursive2/assignment.tbl --max-num-provers 100 -e pallas --check
#ninja -C build_recursive placeholder_verifier_cpp_example
