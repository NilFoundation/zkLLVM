# zkLLVM

zkLLVM is a compiler from high-level programming languages into an input for provable computations protocols. 
It can generate input for any arbitrary zero-knowledge proof system or protocol, which accepts 
input data in form of algebraic circuits. It should be used together with `Placeholder` proof system or 
any other arithmetization that is compatible with this system.

**Notice**: zkLLVM is **NOT** a virtual machine and does not use one.
Moreover, the existence of zkLLVM proves that zkVMs are redundant and not necessary.

zkLLVM is designed as an extension to the LLVM toolchain.
It supports any front end which compiles to [LLVM IR](#need-link).
This enables developers to write code in native languages instead of DSLs specific to other libraries.

zkLLVM extends:

1.  `clang/clang++` -- compiles a program into general intermediate representation byte-code from C/C++.
2.  `assigner` -- creates a circuit execution trace (a.k.a. assignment table) and produces data, 
    that a prover can use to produce the proof.

zkLLVM supports the following languages:

1.  C/C++ -- all standards that Clang 15 supports.
2.  Rust -- coming soon. 

Your language suggestions are warmly welcomed in our [Telegram] or on [Discord].

[Telegram]: https://t.me/nilfoundation
[Discord]: https://discord.gg/KmTAEjbmM3

## Building

### Unix

#### 0. Install Dependencies

On \*nix systems, the following dependencies need to be present:

*   [Boost](https://www.boost.org/) >= 1.74.0
*   [CMake](https://cmake.org/) >= 3.5
*   [Clang](https://clang.llvm.org/) >= 14.0.6

```bash
apt install build-essential libssl-dev libboost-all-dev cmake clang git
```

#### 1. Clone the repository

Clone the repository and all the submodules:

```bash
git clone --recurse-submodules git@github.com:NilFoundation/zkllvm.git
cd zkllvm
```

#### 2. Configure CMake

```bash
cmake -G "Unix Makefiles" -B ${ZKLLVM_BUILD:-build} -DCMAKE_BUILD_TYPE=Release .
```

#### 3. Build the compiler

```bash
make -C ${ZKLLVM_BUILD:-build} assigner clang -j$(nproc)
```

## Usage

zkLLVM's workflow is as follows:

1.  Users wishing to prove a statement should implement an application in a language
    compatible with some frontend (C++ for now).
    This code will be compiled with a modified version of the `clang` compiler,
    which will output an intermediate representation of the circuit. 

    > We recommend using [=nil; Foundation's Crypto3 library](https://github.com/nilfoundation/crypto3.git)
    > for the most performant cryptography circuits: hashes, signatures, VDFs, proof system verifications, and so on.

2.  Users put an order for proof on the [=nil; Foundation's Proof Market](https://proof.market.nil.foundation).
3.  Proof generators pick up the orders and generate proofs.

An order consists of two binary files:

*   Constraint representing the arithmetization of the circuit.
*   Assignment Table contains pre-processed public inputs and witness.

The constraint and assignment table are passed as in input to proof generator binary,
which then outputs a binary proof file.

Proof verification is not a part of the zkLLVM project.
It involves a few more steps which require circuit serialization 
to be set up on the blockchain.

### Examples

#### Linux 

```bash
make -C ${ZKLLVM_BUILD:-build} circuit_examples -j$(nproc) 
${ZKLLVM_BUILD:-build}/bin/assigner/assigner \
  -b ${ZKLLVM_BUILD:-build}/examples/arithmetics_example.bc \
  -i examples/arithmetics.inp \
  -t assignment.tbl \
  -c circuit.crct
```

#### macOS

```bash
make -C ${ZKLLVM_BUILD:-build} circuit_examples -j$(sysctl -n hw.logicalcpu)
${ZKLLVM_BUILD:-build}/bin/assigner/assigner \
  -b ${ZKLLVM_BUILD:-build}/examples/arithmetics_example.bc \
  -i examples/arithmetics.inp \
  -t assignment.tbl \
  -c circuit.crct
```