---
description: Compile zero-knowledge proof systems in high-level programming languages
---

# zkLLVM

## `=nil;` zkLLVM Project

zkLLVM is a compiler from high-level programming languages into input for provable computation protocols.
Developers can use zkLLVM to generate inputs for any arbitrary zero-knowledge proof system or protocol
that accepts input data in the form of algebraic circuits.
It can be used with the Placeholder proof system — a derivative of PLONK —
or any other compatible arithmetization.

zkLLVM is an extension to the LLVM toolchain, supporting any frontend that compiles to
[LLVM intermediate representation](https://llvm.org/docs/LangRef.html#introduction).
With zkLLVM, developers can write code in native languages instead of DSLs specific to other tools.

zkLLVM toolchain has two main tools:

1.  `clang` compiles the program into a circuit — an intermediate representation in bytecode.
2.  `assigner` creates an assignment table using the compiled circuit and input data.

### Zero-knowledge proof workflow with zkLLVM

zkLLVM toolchain is a crucial part of the zero-knowledge proof verification workflow.
Here's how it all works.

1.  First, the companies who want to prove transactions develop their code in a compatible language, like C++.
    They compile it with zkLLVM into a circuit — a provable bytecode representation of the application.
    
    {% hint style="info" %}
    `=nil;` recommends using [Crypto3 library](https://docs.nil.foundation/crypto3/)
    for developing most performant circuit components.
    {% endhint %}

1.  To prove a particular transaction, the proof requester
    combines the circuit with the inputs of that transaction in the `assigner`
    and gets two binary files:
    
    * Constraint, which represents the arithmetization of the circuit.
    * Assignment table, which contains pre-processed public inputs.

3.  Proof producers use these files to verify the proof.
    They return a verified proof to the requester if the form of another binary file.
    There are two general approaches to verification: on-chain and off-chain:
    
    *   On-chain verification requires serialisation of the circuit and is deployed on blockchain clusters.
        The [lorem-ipsum](https://github.com/NilFoundation/lorem-ipsum-cli) project
        handles this flow of generating smart contracts.
    *   Support for off-chain proof verification with `=nil;` tools will be added in the future.

For more details about the workflow, read the guides for [circuit developers](manual/getting-started/circuit-generation.md)
and [proof verifiers](manual/getting-started/proof-verifier.md).