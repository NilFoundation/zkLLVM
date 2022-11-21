---
description: Compile zero knowledge proof systems in high-level programming languages
---

# zkLLVM

## =nil; zkLLVM Project

zkLLVM is compiler from high-level programming languages into input for provable computations protocols. It can be used to generate input for any arbitrary zero-knowledge proof system or protocol, which accepts input data in form of algebraic circuits.It is assumed to be used together with `placeholder` proof system or any arithmetization compatible with `placeholder;` which is a derivative of PLONK proof system.

It is designed as an extension to LLVM tool chain , thus supports any front end which compiles to LLVM IR. This enables developers to write code in native language instead of DLS's specific to other libraries.

zkLLVM tool chain adds extensions via:&#x20;

1. `clang` : Compiles the program into intermediate representation byte-code.
2. `assigner` : Creates the assignment table.

### Tool Chain

Below we look at flow of how the zkLLVM tool chain is invoked:

1. Users who wish to **generate a provable circuit**, will write their code in a compatible front end ex: C++. This code will be compiled with the modified version of the `clang` compiler , which will output byte-code representation of the circuit.  For the most performant circuit components  we recommend using crypto3 library , however this is not essential.
2.  Users who wish to **generate a proof for the circuit** , will require to run the `assigner` along with the circuit generated above and pass the public inputs & witness (if necessary). This will output two files:

    * Constraint : Binary file representing arithmetization of the circuit.
    * Assignment Table: Binary file pre-processed with public inputs & witness.

    The constraint and assignment table generated above should be passed as in input to proof generator binary. (TODO add more details). This will output a binary proof file.
3. Proof verification is not part of the zkLLVM tool-chain. This involves a more steps which requires serialisation of the circuit and deployed on blockchain clusters. (TODO add example)
