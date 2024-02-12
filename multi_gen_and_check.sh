set -e
mkdir -p ./recursive
ninja -C build merkle_poseidon_3provers_cpp_example_assign -j4
./build/bin/recursive_gen/recursive_gen -m gen-input -i ./examples/inputs/merkle_poseidon_3provers_public.inp -t ./build/examples/cpp/assignment_merkle_poseidon_3provers_cpp_example.tbl0 -c ./build/examples/cpp/circuit_merkle_poseidon_3provers_cpp_example.crct0 -o ./recursive -e pallas -p 1 -s 3 --multi-prover
./build/bin/recursive_gen/recursive_gen -m gen-verifier -i ./examples/inputs/merkle_poseidon_3provers_public.inp -t ./build/examples/cpp/assignment_merkle_poseidon_3provers_cpp_example.tbl0 -c ./build/examples/cpp/circuit_merkle_poseidon_3provers_cpp_example.crct0 -o ./recursive -e pallas -p 1 -s 3 --multi-prover

cp -rf ./recursive/placeholder_verifier.cpp ./examples/cpp/placeholder_verifier.cpp
cp -rf ./recursive/placeholder_verifier.inp ./examples/inputs/placeholder_verifier.inp

ninja -C build placeholder_verifier_cpp_example -j4
./build_recursive/bin/assigner/assigner -i ./examples/inputs/placeholder_verifier.inp -b ./build/examples/cpp/placeholder_verifier_cpp_example.ll -c recursive/circuit.crct -t recursive/assignment.tbl --max-num-provers 100 -e pallas --check

./build/bin/recursive_gen/recursive_gen -m gen-input -i ./examples/inputs/merkle_poseidon_3provers_public.inp -t ./build/examples/cpp/assignment_merkle_poseidon_3provers_cpp_example.tbl1 -c ./build/examples/cpp/circuit_merkle_poseidon_3provers_cpp_example.crct1 -o ./recursive -e pallas -p 1 -s 3 --multi-prover
./build/bin/recursive_gen/recursive_gen -m gen-verifier -i ./examples/inputs/merkle_poseidon_3provers_public.inp -t ./build/examples/cpp/assignment_merkle_poseidon_3provers_cpp_example.tbl1 -c ./build/examples/cpp/circuit_merkle_poseidon_3provers_cpp_example.crct1 -o ./recursive -e pallas -p 1 -s 3 --multi-prover

cp -rf ./recursive/placeholder_verifier.cpp ./examples/cpp/placeholder_verifier.cpp
cp -rf ./recursive/placeholder_verifier.inp ./examples/inputs/placeholder_verifier.inp

ninja -C build placeholder_verifier_cpp_example -j4
./build_recursive/bin/assigner/assigner -i ./examples/inputs/placeholder_verifier.inp -b ./build/examples/cpp/placeholder_verifier_cpp_example.ll -c recursive/circuit.crct -t recursive/assignment.tbl --max-num-provers 100 -e pallas --check

./build/bin/recursive_gen/recursive_gen -m gen-input -i ./examples/inputs/merkle_poseidon_3provers_public.inp -t ./build/examples/cpp/assignment_merkle_poseidon_3provers_cpp_example.tbl2 -c ./build/examples/cpp/circuit_merkle_poseidon_3provers_cpp_example.crct2 -o ./recursive -e pallas -p 1 -s 3 --multi-prover
./build/bin/recursive_gen/recursive_gen -m gen-verifier -i ./examples/inputs/merkle_poseidon_3provers_public.inp -t ./build/examples/cpp/assignment_merkle_poseidon_3provers_cpp_example.tbl2 -c ./build/examples/cpp/circuit_merkle_poseidon_3provers_cpp_example.crct2 -o ./recursive -e pallas -p 1 -s 3 --multi-prover

cp -rf ./recursive/placeholder_verifier.cpp ./examples/cpp/placeholder_verifier.cpp
cp -rf ./recursive/placeholder_verifier.inp ./examples/inputs/placeholder_verifier.inp

ninja -C build placeholder_verifier_cpp_example -j4
./build_recursive/bin/assigner/assigner -i ./examples/inputs/placeholder_verifier.inp -b ./build/examples/cpp/placeholder_verifier_cpp_example.ll -c recursive/circuit.crct -t recursive/assignment.tbl --max-num-provers 100 -e pallas --check
