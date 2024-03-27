set -e
mkdir -p ./recursive
ninja -C build merkle_poseidon_3provers_cpp_example_generate_both -j4

# Prover 0
rm -rf ./recursive/multi-gen0
mkdir -p ./recursive/multi-gen0

# Generate proof and verifier
./build/bin/recursive_gen/recursive_gen -m gen-input -i ./examples/inputs/merkle_poseidon_3provers_public.inp -t ./build/examples/cpp/assignment_merkle_poseidon_3provers_cpp_example.tbl0 -c ./build/examples/cpp/circuit_merkle_poseidon_3provers_cpp_example.crct0 -o ./recursive/multi-gen0 -e pallas
./build/bin/recursive_gen/recursive_gen -m gen-verifier -i ./examples/inputs/merkle_poseidon_3provers_public.inp -t ./build/examples/cpp/assignment_merkle_poseidon_3provers_cpp_example.tbl0 -c ./build/examples/cpp/circuit_merkle_poseidon_3provers_cpp_example.crct0 -o ./recursive/multi-gen0 -e pallas

# Assign it
./build/libs/circifier/llvm/bin/clang-17 -target assigner -Xclang -fpreserve-vec3-type -Werror=unknown-attributes -D_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION -D__ZKLLVM__ -I./libs/crypto3/libs/algebra/include -I./build/include -I/usr/local/include -I -I./libs/crypto3/libs/block/include -I/usr/local/include -I./libs/blueprint/include -I./libs/crypto3/libs/codec/include -I./libs/crypto3/libs/containers/include -I./libs/crypto3/libs/hash/include -I./libs/crypto3/libs/kdf/include -I./libs/crypto3/libs/mac/include -I./libs/crypto3/libs/marshalling/core/include -I./libs/crypto3/libs/marshalling/algebra/include -I./libs/crypto3/libs/marshalling/multiprecision/include -I./libs/crypto3/libs/marshalling/zk/include -I./libs/crypto3/libs/math/include -I./libs/crypto3/libs/modes/include -I./libs/crypto3/libs/multiprecision/include -I./libs/crypto3/libs/passhash/include -I./libs/crypto3/libs/pbkdf/include -I./libs/crypto3/libs/threshold/include -I./libs/crypto3/libs/pkpad/include -I./libs/crypto3/libs/pubkey/include -I./libs/crypto3/libs/random/include -I./libs/crypto3/libs/stream/include -I./libs/crypto3/libs/vdf/include -I./libs/crypto3/libs/zk/include -I./libs/stdlib/libcpp -I./libs/circifier/clang/lib/Headers -I./libs/stdlib/libc/include -emit-llvm -O1 -S -o ./recursive/multi-gen0/placeholder_verifier.ll ./recursive/multi-gen0/placeholder_verifier.cpp
./build/bin/assigner/assigner -i ./recursive/multi-gen0/placeholder_verifier.inp -b ./recursive/multi-gen0/placeholder_verifier.ll -c recursive/multi-gen0/circuit.crct -t recursive/multi-gen0/assignment.tbl -e pallas --check  --column-sizes {15,1,2,0,30,0}

# Prover 1
rm -rf ./recursive/multi-gen1
mkdir -p ./recursive/multi-gen1

# Generate proof and verifier
./build/bin/recursive_gen/recursive_gen -m gen-input -i ./examples/inputs/merkle_poseidon_3provers_public.inp -t ./build/examples/cpp/assignment_merkle_poseidon_3provers_cpp_example.tbl1 -c ./build/examples/cpp/circuit_merkle_poseidon_3provers_cpp_example.crct1 -o ./recursive/multi-gen1 -e pallas
./build/bin/recursive_gen/recursive_gen -m gen-verifier -i ./examples/inputs/merkle_poseidon_3provers_public.inp -t ./build/examples/cpp/assignment_merkle_poseidon_3provers_cpp_example.tbl1 -c ./build/examples/cpp/circuit_merkle_poseidon_3provers_cpp_example.crct1 -o ./recursive/multi-gen1 -e pallas

# Assign it
./build/libs/circifier/llvm/bin/clang-17 -target assigner -Xclang -fpreserve-vec3-type -Werror=unknown-attributes -D_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION -D__ZKLLVM__ -I./libs/crypto3/libs/algebra/include -I./build/include -I/usr/local/include -I -I./libs/crypto3/libs/block/include -I/usr/local/include -I./libs/blueprint/include -I./libs/crypto3/libs/codec/include -I./libs/crypto3/libs/containers/include -I./libs/crypto3/libs/hash/include -I./libs/crypto3/libs/kdf/include -I./libs/crypto3/libs/mac/include -I./libs/crypto3/libs/marshalling/core/include -I./libs/crypto3/libs/marshalling/algebra/include -I./libs/crypto3/libs/marshalling/multiprecision/include -I./libs/crypto3/libs/marshalling/zk/include -I./libs/crypto3/libs/math/include -I./libs/crypto3/libs/modes/include -I./libs/crypto3/libs/multiprecision/include -I./libs/crypto3/libs/passhash/include -I./libs/crypto3/libs/pbkdf/include -I./libs/crypto3/libs/threshold/include -I./libs/crypto3/libs/pkpad/include -I./libs/crypto3/libs/pubkey/include -I./libs/crypto3/libs/random/include -I./libs/crypto3/libs/stream/include -I./libs/crypto3/libs/vdf/include -I./libs/crypto3/libs/zk/include -I./libs/stdlib/libcpp -I./libs/circifier/clang/lib/Headers -I./libs/stdlib/libc/include -emit-llvm -O1 -S -o ./recursive/multi-gen1/placeholder_verifier.ll ./recursive/multi-gen1/placeholder_verifier.cpp
./build/bin/assigner/assigner -i ./recursive/multi-gen1/placeholder_verifier.inp -b ./recursive/multi-gen1/placeholder_verifier.ll -c recursive/multi-gen1/circuit.crct -t recursive/multi-gen1/assignment.tbl -e pallas --check  --column-sizes {15,1,2,0,30,0}

# Prover 1
rm -rf ./recursive/multi-gen2
mkdir -p ./recursive/multi-gen2

# Generate proof and verifier
./build/bin/recursive_gen/recursive_gen -m gen-input -i ./examples/inputs/merkle_poseidon_3provers_public.inp -t ./build/examples/cpp/assignment_merkle_poseidon_3provers_cpp_example.tbl2 -c ./build/examples/cpp/circuit_merkle_poseidon_3provers_cpp_example.crct2 -o ./recursive/multi-gen2 -e pallas
./build/bin/recursive_gen/recursive_gen -m gen-verifier -i ./examples/inputs/merkle_poseidon_3provers_public.inp -t ./build/examples/cpp/assignment_merkle_poseidon_3provers_cpp_example.tbl2 -c ./build/examples/cpp/circuit_merkle_poseidon_3provers_cpp_example.crct2 -o ./recursive/multi-gen2 -e pallas

# Assign it
./build/libs/circifier/llvm/bin/clang-17 -target assigner -Xclang -fpreserve-vec3-type -Werror=unknown-attributes -D_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION -D__ZKLLVM__ -I./libs/crypto3/libs/algebra/include -I./build/include -I/usr/local/include -I -I./libs/crypto3/libs/block/include -I/usr/local/include -I./libs/blueprint/include -I./libs/crypto3/libs/codec/include -I./libs/crypto3/libs/containers/include -I./libs/crypto3/libs/hash/include -I./libs/crypto3/libs/kdf/include -I./libs/crypto3/libs/mac/include -I./libs/crypto3/libs/marshalling/core/include -I./libs/crypto3/libs/marshalling/algebra/include -I./libs/crypto3/libs/marshalling/multiprecision/include -I./libs/crypto3/libs/marshalling/zk/include -I./libs/crypto3/libs/math/include -I./libs/crypto3/libs/modes/include -I./libs/crypto3/libs/multiprecision/include -I./libs/crypto3/libs/passhash/include -I./libs/crypto3/libs/pbkdf/include -I./libs/crypto3/libs/threshold/include -I./libs/crypto3/libs/pkpad/include -I./libs/crypto3/libs/pubkey/include -I./libs/crypto3/libs/random/include -I./libs/crypto3/libs/stream/include -I./libs/crypto3/libs/vdf/include -I./libs/crypto3/libs/zk/include -I./libs/stdlib/libcpp -I./libs/circifier/clang/lib/Headers -I./libs/stdlib/libc/include -emit-llvm -O1 -S -o ./recursive/multi-gen2/placeholder_verifier.ll ./recursive/multi-gen2/placeholder_verifier.cpp
./build/bin/assigner/assigner -i ./recursive/multi-gen2/placeholder_verifier.inp -b ./recursive/multi-gen2/placeholder_verifier.ll -c recursive/multi-gen2/circuit.crct -t recursive/multi-gen2/assignment.tbl -e pallas --check  --column-sizes {15,1,2,0,30,0}
