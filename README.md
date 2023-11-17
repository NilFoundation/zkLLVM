# zkLLVM Circuit Compiler

[![build-linux](https://github.com/NilFoundation/zkllvm/actions/workflows/build_linux.yml/badge.svg?branch=master)](
https://github.com/NilFoundation/zkllvm/actions/workflows/build_linux.yml)
[![build-macos](https://github.com/NilFoundation/zkllvm/actions/workflows/build_macos.yml/badge.svg?branch=master)](
https://github.com/NilFoundation/zkllvm/actions/workflows/build_macos.yml)

[![Discord](https://img.shields.io/discord/969303013749579846.svg?logo=discord&style=flat-square)](https://discord.gg/KmTAEjbmM3)
[![Telegram](https://img.shields.io/badge/Telegram-2CA5E0?style=flat-square&logo=telegram&logoColor=dark)](https://t.me/nilfoundation)
[![Twitter](https://img.shields.io/twitter/follow/nil_foundation)](https://twitter.com/nil_foundation)

zkLLVM is a compiler from high-level programming languages into an input for provable computations protocols.
It can be used to generate input for any arbitrary zero-knowledge proof system or protocol, which accepts
input data in form of algebraic circuits It assumed to be used together with `Placeholder` proof system or
any other arithmetization compatible with `Placeholder` proof system.

Every proof output from zkLLVM is **an in-EVM verifiable** one through the [Proof Market](https://proof.market). Use the Proof Market Toolchain repository (https://github.com/NilFoundation/proof-market-toolchain) to generate in-EVM verifiers.

**Notice**: zkLLVM is **NOT** a virtual machine and has nothing to do with it. It, moreover, with its existence proves the absence of necessity in zkVMs, posing them as redundant.

zkLLVM is designed as an extension to LLVM toolchain, thus supports any front end which compiles to LLVM IR. This
enables developers to write code in native language instead of DSL's specific to other libraries.

zkLLVM extends:
1. `clang/clang++` : Compiles the program into general intermediate representation byte-code from C/C++.
2. `rustc`: Compiles the program into general intermediate representation byte-code from Rust. (https://github.com/NilFoundation/zkllvm-rslang)
3. `assigner` Creates the circuit execution trace (a.k.a. assignment table) and produces data, needed by the prover to produce proof.

Languages currently supported are:
1. C/C++ (all the standards Clang 15 supports).
2. Rust (https://github.com/NilFoundation/zkllvm-rslang).
3. Your language suggestions are warmly welcomed in Telegram (https://t.me/nilfoundation) or on Discord (https://discord.gg/KmTAEjbmM3).

## Building

### Unix

#### Install Dependencies

* [Boost](https://www.boost.org/) >= 1.76.0
* [CMake](https://cmake.org/) >= 3.5
* [Clang](https://clang.llvm.org/) >= 12.0
* [Python](https://www.python.org/) >= 3.7
* [Rust](https://www.rust-lang.org/) >= 1.68 (if you want to build Rust compiler)

On Debian systems, everything except Boost and Rust can be installed with the following command:

```
sudo apt install build-essential libssl-dev cmake clang-12 git curl pkg-config libspdlog-dev
```

#### 1. Clone the repository

Clone the repository and all the submodules via:

```
git clone --recurse-submodules https://github.com/NilFoundation/zkLLVM.git
cd zkLLVM
```

#### **2. Configure CMake**

```bash
cmake -G "Unix Makefiles" -B ${ZKLLVM_BUILD:-build} -DCMAKE_BUILD_TYPE=Release .
```
Or use the below command instead, if you prefer Ninja build system (as we do, because Ninja works much faster).

```bash
cmake -G "Ninja" -B ${ZKLLVM_BUILD:-build} -DCMAKE_BUILD_TYPE=Release .
```

#### **3. Build C++ compiler**

If you are using Unix makefiles:
```bash
make -C ${ZKLLVM_BUILD:-build} assigner clang -j$(nproc)
```
If you are using Ninja:

```bash
ninja -C ${ZKLLVM_BUILD:-build} assigner clang -j$(nproc)
```


#### **4. Build Rust compiler**

Make sure you have [`rustc`](https://www.rust-lang.org/tools/install) with `cargo` installed first.

##### **4.1 Reconfigure CMake, adding Rust tools**

Unix makefiles:
```bash
cmake -G "Unix Makefiles" -B ${ZKLLVM_BUILD:-build} -DCMAKE_BUILD_TYPE=Release -DRSLANG_BUILD_EXTENDED=TRUE -DRSLANG_BUILD_TOOLS=cargo .
```

Ninja:
```bash
cmake -G "Ninja" -B ${ZKLLVM_BUILD:-build} -DCMAKE_BUILD_TYPE=Release -DRSLANG_BUILD_EXTENDED=TRUE -DRSLANG_BUILD_TOOLS=cargo .
```

##### **4.2 Export path for loading LLVM libraries**

```bash
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$(pwd)/build/libs/circifier/llvm/lib"
```

##### **4.3 Build Rust compiler and Cargo**

Unix makefiles:
```bash
make -C ${ZKLLVM_BUILD:-build} rslang -j$(nproc)
```

Ninja:
```bash
ninja -C ${ZKLLVM_BUILD:-build} rslang -j$(nproc)
```

After that you will be able to call Cargo like that:

```bash
export RSLANG="$(pwd)/build/libs/rslang/build/host"
RUSTC=$RSLANG/stage1/bin/rustc $RSLANG/stage1-tools-bin/cargo --version
```

Note: if you want an advanced Rust compilation, you can build `zkllvm` first:

Unix makefiles:
```bash
make -C ${ZKLLVM_BUILD:-build} -j$(nproc)
```

Ninja:
```bash
ninja -C ${ZKLLVM_BUILD:-build} -j$(nproc)
```

And then use Rust default build system `x.py`.

## Pre-built binaries

### Rust toolchain

You can install `rslang` as another toolchain in [`rustup`](https://rustup.rs/) or as a standalone application.
Installation is done via `rslang-installer.py` script. It finds and downloads required release of `rslang`
and installs it in the desired location.

#### **Supported platforms**

* x86-64 GNU/Linux

#### **Prerequisites**

* Python 3.7+

#### **Installation with `rustup`**

1. Install `rustup` as described on [official page](https://rustup.rs/).

2. Install `rslang`:

Run this in your shell:

```bash
curl --proto '=https' --tlsv1.2 -sSf https://cdn.jsdelivr.net/gh/NilFoundation/zkllvm@master/rslang-installer.py | python - --channel nightly
```

You can also download the `rslang-installer.py` first and then run it:

```bash
curl --proto '=https' --tlsv1.2 -O https://cdn.jsdelivr.net/gh/NilFoundation/zkllvm@master/rslang-installer.py
python rslang-installer.py --channel nightly
```

Now you can use toolchain called `zkllvm` to compile with `rslang`:

```bash
rustc +zkllvm -V
```

#### **Stanalone installation**

To install `rslang` without `rustup` use `--no-rustup` argument.
You will need to pass `PATH` to desired installation directory.

```bash
curl --proto '=https' --tlsv1.2 -O https://cdn.jsdelivr.net/gh/NilFoundation/zkllvm@master/rslang-installer.py
python rslang-installer.py --no-rustup --prefix PATH
```

## Usage

zkLLVM's workflow is as follows:

1. **Write Circuit :** Users willing to prove some statement are supposed to implement an application in a language compatible with some frontend (C++ for now). This code will be compiled with a modified version of the `clang` compiler, which will output intermediate representation of the circuit.
   ![compile](./docs/assets/compile.png)
   > For the most performant cryptography circuits (e.g. hashes, signatures, VDFs, proof system verifications, etc.)
   > we recommend using [=nil; Foundation's Crypto3 library](https://github.com/nilfoundation/crypto3.git).

   ![compile](./docs/assets/transpile.png)
  The circuit developer will be generating the in-EVM applications for the circuits they have created. This will enable on-chain verification of the proof.
  The in-EVM logic consists of gate representations of the circuit. These contracts work in conjunction with the Placeholder proof validation in-EVM logic.
  The process to transpile the circuit into smart contracts is handled by the [lorem-ipsum](https://github.com/NilFoundation/lorem-ipsum-cli)
  project.

2. **Publish Circuit/Generate Proof**: zkLLVM is tightly coupled with [=nil; Foundation's Proof Market](https://proof.market.nil.foundation). Users willing to generate a proof for the circuit, will be matched with counter-parties based on price and other conditions.
   The circuit generated above needs to be published to proof market to enable this.
   ![publish](./docs/assets/publish.png)

To generate a proof it is required to pass the following to the proof generator:

    * Circuit : Arithmetization of the circuit.
    * Inputs: Public (and private) inputs to circuit part of the proof request.

This generates the binary proof file. This flow is handled by the [proof market toolchain](https://github.com/NilFoundation/proof-market-toolchain) repository & documented [here](https://docs.nil.foundation/proof-market/market/user-guides/proof-producer).

Users can generate & inspect intermediate artifacts such as execution trace by running the `assigner` process. See examples below.

3. **Verify Proof**: Proof can be retrieved from the proof market and verified on chain. Users can verify proof in these modes :
   1. Offline : Tooling to support validation of off-chain proof will be added in the future.
   2. On-chain : This flow of generating smart contracts is handled by the [lorem-ipsum](https://github.com/NilFoundation/lorem-ipsum-cli) project. A high level flow is described in the guides
   for [circuit developer](https://docs.nil.foundation/zkllvm/manual/getting-started/circuit-generation) & [proof verifier](https://docs.nil.foundation/zkllvm/manual/getting-started/proof-verifier)
   described above.
    ![verify](./docs/assets/dapp_verify.png)

   Above we see how a dApp can use generated verifiers on-chain by simply including verification interfaces.

### Examples

#### Linux

Unix makefiles:
```bash
make -C ${ZKLLVM_BUILD:-build} circuit_cpp_examples -j$(nproc)
${ZKLLVM_BUILD:-build}/bin/assigner/assigner -b ${ZKLLVM_BUILD:-build}/examples/cpp/arithmetics_cpp_example.ll -i examples/inputs/arithmetics.inp -t assignment.tbl -c circuit.crct -e pallas
```

Ninja:
```bash
ninja -C ${ZKLLVM_BUILD:-build} circuit_cpp_examples -j$(nproc)
${ZKLLVM_BUILD:-build}/bin/assigner/assigner -b ${ZKLLVM_BUILD:-build}/examples/cpp/arithmetics_cpp_example.ll -i examples/inputs/arithmetics.inp -t assignment.tbl -c circuit.crct -e pallas
```

#### macOS
Unix makefiles:
```bash
make -C ${ZKLLVM_BUILD:-build} circuit_cpp_examples -j$(sysctl -n hw.logicalcpu)
${ZKLLVM_BUILD:-build}/bin/assigner/assigner -b ${ZKLLVM_BUILD:-build}/examples/cpp/arithmetics_cpp_example.ll -i examples/inputs/arithmetics.inp -t assignment.tbl -c circuit.crct -e pallas
```

Ninja:
```bash
ninja -C ${ZKLLVM_BUILD:-build} circuit_cpp_examples -j$(sysctl -n hw.logicalcpu)
${ZKLLVM_BUILD:-build}/bin/assigner/assigner -b ${ZKLLVM_BUILD:-build}/examples/cpp/arithmetics_cpp_example.ll -i examples/inputs/arithmetics.inp -t assignment.tbl -c circuit.crct -e pallas
```

### Validating the circuit

You can also run the `assigner` with `--check` flag to validate the satisfiability of the circuit. If the circuit is satisfiable, the `assigner` will output the satisfying assignment in the `assignment.tbl` file. If there is an error, the `assigner` will output the error message and throw an exception via `std::abort`.

#### Linux
Unix makefiles:
```bash
make -C ${ZKLLVM_BUILD:-build} circuit_cpp_examples -j$(nproc)
${ZKLLVM_BUILD:-build}/bin/assigner/assigner -b ${ZKLLVM_BUILD:-build}/examples/cpp/arithmetics_cpp_example.ll -i examples/inputs/arithmetics.inp -t assignment.tbl -c circuit.crct -e pallas --check
```

Ninja:
```bash
ninja -C ${ZKLLVM_BUILD:-build} circuit_cpp_examples -j$(nproc)
${ZKLLVM_BUILD:-build}/bin/assigner/assigner -b ${ZKLLVM_BUILD:-build}/examples/cpp/arithmetics_cpp_example.ll -i examples/inputs/arithmetics.inp -t assignment.tbl -c circuit.crct -e pallas --check
```

#### macOS
Unix makefiles:
```bash
make -C ${ZKLLVM_BUILD:-build} circuit_cpp_examples -j$(sysctl -n hw.logicalcpu)
${ZKLLVM_BUILD:-build}/bin/assigner/assigner -b ${ZKLLVM_BUILD:-build}/examples/cpp/arithmetics_cpp_example.ll -i examples/inputs/arithmetics.inp -t assignment.tbl -c circuit.crct -e pallas --check
```

Ninja:
```bash
ninja -C ${ZKLLVM_BUILD:-build} circuit_cpp_examples -j$(sysctl -n hw.logicalcpu)
${ZKLLVM_BUILD:-build}/bin/assigner/assigner -b ${ZKLLVM_BUILD:-build}/examples/cpp/arithmetics_cpp_example.ll -i examples/inputs/arithmetics.inp -t assignment.tbl -c circuit.crct -e pallas --check
```