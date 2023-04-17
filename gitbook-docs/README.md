---
description: Compile zero knowledge proof systems in high-level programming languages
---

# zkLLVM

## =nil; zkLLVM Project

zkLLVM is a compiler from high-level programming languages into input for provable computations protocols. It can be used to generate input for any arbitrary zero-knowledge proof system or protocol, which accepts input data in form of algebraic circuits. It is assumed to be used together with `placeholder` proof system or any arithmetization compatible with `placeholder` which is a derivative of the PLONK proof system.

It is designed to extend the LLVM toolchain, thus supporting any front end that compiles to LLVM IR. This enables developers to write code in their native language instead of DLS's specific to other libraries.

zkLLVM toolchain adds extensions via:&#x20;

1. `clang` : Compiles the program into intermediate representation byte-code.
2. `assigner` : Creates the assignment table for the circuit.

### Tool Chain

Below we look at the flow of how the zkLLVM toolchain is invoked:

1. Users who wish to **generate a provable circuit** will write their code in a compatible front end ex: C++. This code will be compiled with the modified version of the `clang` compiler, which will output the byte-code representation of the circuit.  For the most performant circuit components, we recommend using the crypto3 library; however, this is not essential.
2.  Users who wish to **generate proof for the circuit** will require to run  `assigner` along with the circuit generated above and pass the public inputs & witness (if necessary). This will output two files:

    * Constraint: Binary file representing arithmetization of the circuit.
    * Assignment Table: Binary file pre-processed with public inputs & witness.

    The constraint and assignment table generated above should be passed as input to the proof generator binary. This will output a binary proof file.
3. Proof verification is not part of the zkLLVM toolchain currently, but can be done via:&#x20;
   1. Offline: Tooling to support validation of off-chain proof will be added in future.
   2. On-chain: This involves more steps requiring circuit serialisation and deployment on blockchain clusters. This flow of generating smart contracts is handled by the [lorem-ipsum](https://github.com/NilFoundation/lorem-ipsum-cli) project. A high-level flow is described in the guides for [circuit developers](manual/getting-started/circuit-generation.md) & [proof verifiers](manual/getting-started/proof-verifier.md).
