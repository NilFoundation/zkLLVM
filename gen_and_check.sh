set -e
mkdir -p ./recursive
./build/bin/recursive_gen/recursive_gen -m gen-input -i ./examples/inputs/arithmetics.inp -t ./build/examples/cpp/assignment_arithmetics_cpp_example.tbl -c ./build/examples/cpp/circuit_arithmetics_cpp_example.crct -o ./recursive -e pallas
./build/bin/recursive_gen/recursive_gen -m gen-verifier -i ./examples/inputs/arithmetics.inp -t ./build/examples/cpp/assignment_arithmetics_cpp_example.tbl -c ./build/examples/cpp/circuit_arithmetics_cpp_example.crct -o ./recursive -e pallas

#./build/bin/recursive_gen/recursive_gen -m gen-input -i ./examples/inputs/merkle_tree_poseidon.inp -t ./build/examples/cpp/assignment_merkle_tree_poseidon_cpp_example.tbl -c ./build/examples/cpp/circuit_merkle_tree_poseidon_cpp_example.crct -o ./recursive -e pallas
#./build/bin/recursive_gen/recursive_gen -m gen-verifier -i ./examples/inputs/merkle_tree_poseidon.inp -t ./build/examples/cpp/assignment_merkle_tree_poseidon_cpp_example.tbl -c ./build/examples/cpp/circuit_merkle_tree_poseidon_cpp_example.crct -o ./recursive -e pallas

cp -rf ./recursive/placeholder_verifier.cpp ./examples/cpp/placeholder_verifier.cpp
cp -rf ./recursive/placeholder_verifier.inp ./examples/inputs/placeholder_verifier.inp

ninja -C build placeholder_verifier_cpp_example -j4
./build_recursive/bin/assigner/assigner -i ./examples/inputs/placeholder_verifier.inp -b ./build/examples/cpp/placeholder_verifier_cpp_example.ll -c recursive/circuit.crct -t recursive/assignment.tbl --max-num-provers 100 -e pallas
