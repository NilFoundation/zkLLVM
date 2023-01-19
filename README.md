# zkLLVM Circuit Compiler

[![Discord](https://img.shields.io/discord/969303013749579846.svg?logo=discord&style=flat-square)](https://discord.gg/KmTAEjbmM3)
[![Telegram](https://img.shields.io/badge/Telegram-2CA5E0?style=flat-square&logo=telegram&logoColor=dark)](https://t.me/nilfoundation)
[![Twitter](https://img.shields.io/twitter/follow/nil_foundation)](https://twitter.com/nil_foundation)

zkLLVM is a compiler from high-level programming languages into an input for provable computations protocols. 
It can be used to generate input for any arbitrary zero-knowledge proof system or protocol, which accepts 
input data in form of algebraic circuits It assumed to be used together with `Placeholder` proof system or 
any other arithmetization compatible with `Placeholder` proof system.

**Notice**: zkLLVM is **NOT** a virtual machine and has nothing to do with it. It, moreover, with its existence proves the absence of necessity in zkVMs, posing them as redundant.

zkLLVM is designed as an extension to LLVM toolchain, thus supports any front end which compiles to LLVM IR. This 
enables developers to write code in native language instead of DSL's specific to other libraries.

zkLLVM extends:
1. `clang/clang++` : Compiles the program into general intermediate representation byte-code from C/C++.
2. `assigner` Creates the circuit execution trace (a.k.a. assignment table) and produces data, needed by the prover to produce proof.

Languages currently supported are:
1. C/C++ (all the standards Clang 15 supports).
2. Rust (coming up soon).
3. Your language suggestions are warmly welcomed in Telegram (https://t.me/nilfoundation) or on Discord (https://discord.gg/KmTAEjbmM3).

## Building

### Unix

#### Install Dependencies

* [Boost](https://www.boost.org/) >= 1.74.0
* [CMake](https://cmake.org/) >= 3.5
* [Clang](https://clang.llvm.org/) >= 14.0.6

On \*nix systems, the following dependencies need to be present & can be installed using the following command

```
 sudo apt install build-essential libssl-dev libboost-all-dev cmake clang git
```

#### 1. Clone the repository

Clone the repository and all the submodules via:

```
git clone --recurse-submodules git@github.com:NilFoundation/zkllvm.git
cd zkllvm
```

#### **2. Configure CMake**

```bash
cmake -G "Unix Makefiles" -B ${ZKLLVM_BUILD:-build} -DCMAKE_BUILD_TYPE=Release .
```

**3. Build the compiler**

```bash
make -C ${ZKLLVM_BUILD:-build} assigner clang -j$(nproc)
```

## Usage

zkLLVM's workflow is as follows:

1. Users wishing to prove some statement are supposed to implement an application in a language compatible with some frontend (C++ for now). This code will be compiled with a modified version of the `clang` compiler, which will output intermediate representation of the circuit. 
   > For the most performant cryptography circuits (e.g. hashes, signatures, VDFs, proof system verifications, etc.) 
   > we recommend using [=nil; Foundation's Crypto3 library](https://github.com/nilfoundation/crypto3.git).

2. zkLLVM is tightly coupled with [=nil; Foundation's Proof Market](https://proof.market.nil.foundation) so users willing to generate a 
   proof for the circuit, will be required to put an order for a proof so proof generators could pick it up and 
   generate proofs necessary. To submit an order it is required to: 

    * Retrieve a constraints description from Constraint : Binary file representing arithmetization of the circuit.
    * Assignment Table: Binary file pre-processed public inputs & witness.

   The constraint and assignment table generated above should be passed as in input to proof generator binary. This 
   will output a binary proof file.

3. Proof verification is not part of the zkLLVM project. This involves a few more steps which requires serialization of
   the circuit to be setup on chain.

### Examples

#### Linux 

```bash
make -C ${ZKLLVM_BUILD:-build} circuit_examples -j$(nproc) 
${ZKLLVM_BUILD:-build}/bin/assigner/assigner -b ${ZKLLVM_BUILD:-build}/examples/arithmetics_example.bc -i examples/arithmetics.inp -t assignment.tbl -c circuit.crct
```

#### macOS
```bash
make -C ${ZKLLVM_BUILD:-build} circuit_examples -j$(sysctl -n hw.logicalcpu)
${ZKLLVM_BUILD:-build}/bin/assigner/assigner -b ${ZKLLVM_BUILD:-build}/examples/arithmetics_example.bc -i examples/arithmetics.inp -t assignment.tbl -c circuit.crct
```
