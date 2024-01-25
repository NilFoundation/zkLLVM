set -e
ninja -C build transpiler -j4
ninja -C build arithmetics_cpp_example_evm_verifier -j4
rm -rf ../evm-placeholder-verification/contracts/zkllvm/transpiler_output_arithmetics_cpp_example
cp -rf ./build/examples/cpp/transpiler_output_arithmetics_cpp_example ../evm-placeholder-verification/contracts/zkllvm/transpiler_output_arithmetics_cpp_example

cd ../evm-placeholder-verification
npx hardhat deploy
npx hardhat verify-circuit-proof-all

cd ../zkllvm-config