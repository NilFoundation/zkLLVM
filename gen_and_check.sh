set -e
mkdir -p ./recursive

ninja -C build merkle_tree_poseidon_cpp_example_generate_both
./build/bin/recursive_gen/recursive_gen -m gen-input -i ./examples/inputs/merkle_tree_poseidon.inp -t ./build/examples/cpp/assignment_merkle_tree_poseidon_cpp_example.tbl -c ./build/examples/cpp/circuit_merkle_tree_poseidon_cpp_example.crct -o ./recursive -e pallas
./build/bin/recursive_gen/recursive_gen -m gen-verifier -i ./examples/inputs/merkle_tree_poseidon.inp -t ./build/examples/cpp/assignment_merkle_tree_poseidon_cpp_example.tbl -c ./build/examples/cpp/circuit_merkle_tree_poseidon_cpp_example.crct -o ./recursive -e pallas

cp -rf ./recursive/placeholder_verifier.cpp ./examples/cpp/placeholder_verifier.cpp
cp -rf ./recursive/placeholder_verifier.inp ./examples/inputs/placeholder_verifier.inp

ninja -C build placeholder_verifier_cpp_example
./build/bin/assigner/assigner -i ./examples/inputs/placeholder_verifier.inp -b ./build/examples/cpp/placeholder_verifier_cpp_example.ll -c recursive/circuit.crct -t recursive/assignment.tbl --max-num-provers 100 -e pallas --check --column-sizes {15,1,2,0,60,0}