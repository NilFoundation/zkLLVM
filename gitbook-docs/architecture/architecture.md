# Architecture

zkLLVM is designed to be adjustable for different use cases. The architecture is designed to be modular and extensible. It is divided into 3 main components:

* Frontends are responsible for parsing the input high-level language and generating the AST. Currently, we support C/C++ and Rust as frontends which are based on Clang and Rustc respectively. The main difference between the vanilla LLVM project frontends and zkLLVM frontends is that the latter generates the AST in a way that is compatible with the zkSNARKs circuit generation process - it contains additional types and metadata that are required for the circuit generation process.
* `circifier` gets AST and converts it into the intermediate representation (IR) applying circuit-specific optimizations. The output of the circifier is a circuit-specific LLVM IR.
* `assigner` takes the circuit-specific LLVM IR and generates assignment table, using description of the components corresponding to the circuit-specific LLVM IR instructions. `assigner` also generates the arithmetization-specific constraints for the provided LLVM IR instructions. The output of the `assigner` is a set of constraints and the assignment table.

{% hint style="info" %}
A bit later we are going split the assigner logic into two parts: one will be responsible for the assignment table generation and the other will be responsible for the constraints generation. The reason for this is that the constraints generation process can be don one time for the whole circuit, while the assignment table generation process should be done for each input.
{% endhint %}

## Arithmetizations

In order to proceed it may make sense to explain what we assume by the term `arithmetization`, since in different context it might have different meanings. Here we use the term to describe the limitations and the rules of some particular proof system input. The list of currently existing arithmetizations contains but is not limited to:
* R1CS (Rank-1 Constraint System)
* PLONK (Permutation-based zkSNARK)
* PLONK with look-ups
* AIR

Each aritmetization has its own limitation, which have to be taken into account when designing the circuit. Most algorithms can be implemented in different ways, and the different ways can be efficient for different arithmetizations.

As you may notice, arithmetizations are not limited to one proof system. The code compiled to vanilla PLONK arithmetization can be used with different proof systems.