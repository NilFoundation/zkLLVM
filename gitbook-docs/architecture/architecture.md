# Architecture

zkLLVM is designed to be adjustable for different use cases with modular and extendable architecture. It is divided into 3 main components:

* Frontends are responsible for parsing the input high-level language and generating the AST. Currently, we support C/C++ and Rust as frontends which are based on Clang and Rustc respectively. The main difference between the vanilla LLVM project frontends and zkLLVM frontends is that the latter generates the AST in a way that is compatible with the zkSNARKs circuit generation process - it contains additional types and metadata that are required for the circuit generation process.
* `circifier` gets AST and converts it into the intermediate representation (IR) applying circuit-specific optimizations. The output of the circifier is a circuit-specific LLVM IR.
* `assigner` takes the circuit-specific LLVM IR and generates assignment table, using description of the components corresponding to the circuit-specific LLVM IR instructions. `assigner` also generates the arithmetization-specific constraints for the provided LLVM IR instructions. The output of the `assigner` is a set of constraints and the assignment table.

{% hint style="info" %}
We are going split the assigner logic into two parts: one will be responsible for the assignment table generation and the other will be responsible for the constraints generation. The reason for this is that the constraints generation process can be don one time for the whole circuit, while the assignment table generation process should be done for each input.
{% endhint %}

## Arithmetizations

In order to proceed it may make sense to explain what we assume by the term `arithmetization`, since in different context it might have different meanings. Here we use the term to describe the limitations and the rules of some particular proof system input type. The list of currently existing arithmetizations contains but is not limited to:
* R1CS (Rank-1 Constraint System)
* PLONK (Permutation-based zkSNARK)
* PLONK with custom gates
* AIR

Each aritmetization has its own limitation, which have to be taken into account when designing the circuit. Most algorithms can be implemented in different ways, and the different ways can be efficient for different arithmetizations.

As you may notice, arithmetizations are not limited to one proof system. The code compiled to vanilla PLONK arithmetization can be used with different proof systems.

## Extending zkLLVM for new arithmetizations and proof systems

As mentioned above, zkLLVM is designed to be extendable. In order to add support for a new arithmetization, one has to implement only the `assigner` component of the system. It can be implemented in any language, since the core of the `assigner` component is an LLVM IR parser and can be easily ported to any language. Current `assigner` implementation is written in C++ and can be found [here](https://github.com/NilFoundation/zkllvm-assigner) and is only for PLONK with custom gates arithmetization. We are working on the new version of the `assigner` component, which will be written in Rust and will be more generic and will support more arithmetizations. Let's look how the implementation process of this new version will look:
* First, we will implement the LLVM IR parser in Rust. This can be done from scratch or by porting the existing C++ implementation. Or we can use the existing LLVM IR parser from the [inkwell](https://github.com/TheDan64/inkwell) and extend it with the custom types and built-ins.
* Then the rest of assigner logic needs to be implemented. This can be done using an existing RUST PLONK components library, like the components from [halo2](https://github.com/zcash/halo2). You need to solve the following tasks in order to implement minimal assigner for your arithmetization:
  * Assignment table generation
  * Constraints generation
  * Constraints serialization

Apart from the basic functionality, there is a lot of things `assigner` can do to improve the performance of the resulting circuit. For example, it can apply different optimizations to the circuit, like:
* Adjust the order of the components
* Adjust the size of the components in the assigment table
* Group together the components with common constraints (makes sense for PLONKish arithmetizations)

Optimizations in the `assigner` might not be very effective for small circuits, but it can give a significant performance boost for the large ones, such as complex zk-Bridges, zk-VMs or zk-EVMs.

{% hint style="info" %}
An arithmetization is not limited to only one proof system. For example, there are a lot of proof systems with PLONKish arithmetizaion, and the same circuit can be used with all of them. But implementation of the custom `assigner` may be required in case if you work on your own proof system or optimize the existing one. In that case the parameters of the arithmetization might change and the circuit generation has to be adjusted accordingly.
{% endhint %}

You may have noticed, that we only mentioned the `assigner` component, but not the `circifier`. The reason for that is that the `circifier` is not arithmetization-specific and can be used for any arithmetization. It generates the intermediate representation for any circuit generation related process, and the `assigner` component is responsible for the conversion of the intermediate representation into the arithmetization-specific constraints and assignment table.