# zkLLVM

zkLLVM is a compiler from high-level programming languages into an input for provable computations protocols. 
It can be used to generate input for any arbitrary zero-knowledge proof system or protocol, which accepts 
input data in form of algebraic circuits It assumed to be used together with `Placeholder` proof system or 
any other arithmetization compatible with `Placeholder` proof system.

**Notice**: zkLLVM is **NOT** a virtual machine and has nothing to do with it. It, moreover, with its existence proves 

zkLLVM is designed as an extension to LLVM toolchain, thus supports any front end which compiles to LLVM IR. This 
enables developers to write code in native language instead of DSL's specific to other libraries.

zkLLVM extends:
1. `clang/clang++` : Compiles the program into intermediate representation byte-code from C++.
2. `assigner` Creates the assignment table.

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

#### 1. Clone repository

Clone the repository and all sub-modules

```
git clone --recurse-submodules git@github.com:NilFoundation/zkllvm.git
cd zkllvm
```

#### **2. cmake configuration**

```bash
cmake -GNinja -B ${ASSIGNER_BUILD:-build} -DCMAKE_BUILD_TYPE=Debug -DLLVM_ENABLE_PROJECTS=clang .
```

**3. Build clang**&#x20;

```bash
ninja -C ${ASSIGNER_BUILD:-build} assigner -j$(nproc)
```

**4. Build assigner**

```
ninja -C ${ASSIGNER_BUILD:-build} assigner -j$(nproc)
```

## Usage

zkLLVM's workflow is as follows:

1. Users wishing to prove some statement are supposed to implement an application in a language compatible with some frontend (C++ for now). This code will be compiled with a modified version of the `clang` compiler, which will output intermediate representation of the circuit. 
   > For the most performant cryptography circuits (e.g. hashes, signatures, VDFs, proof system verifications, etc.) 
   > we recommend using [=nil; Foundation's Crypto3 library](https://github.com/nilfoundation/crypto3.git).

2. zkLLVM is tightly coupled with [=nil; Foundation's Proof Market](https://proof.market) so users willing to generate a 
   proof for the circuit, will be required to put an order for a proof so proof generators could pick it up and 
   generate proofs necessary. To submit an order it is required to: 

    * Retrieve a constraints description from Constraint : Binary file representing arithmetization of the circuit.
    * Assignment Table: Binary file pre-processed public inputs & witness.

   The constraint and assignment table generated above should be passed as in input to proof generator binary. This 
   will output a binary proof file.

3. Proof verification is not part of the zkLLVM project. This involves a few more steps which requires serialization of
   the circuit to be setup on chain.

### Examples

```bash
${ASSIGNER_BUILD:-build}/libs/circifier/llvm/bin/clang samples/sha512.cpp -emit-llvm -c -O1 -o samples/sha512.bc
${ASSIGNER_BUILD:-build}/bin/assigner samples/sha512.bc -i samples/sha512.inp
```