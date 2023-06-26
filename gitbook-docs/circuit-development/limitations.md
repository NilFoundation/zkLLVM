---
description: Current limitations of the compiler
---

# Limitations

The zkLLVM project is under active development. The current limitations are listed below and will be addressed/updated in future versions.

We are hardly working on removing these limitations, but for now, please keep them in mind when writing circuits. Our final goal is to make the zkLLVM compiler as close to standard C++ as possible.

## Pure functions

The main difference with the usual software development is that you only can use pure functions in your circuits. Because of its polynomial arithmetic nature circuit can't use file system, network, or any other side effects.

## Entry point

The entry point of the project must be marked with `[[circuit]]` directive.
There can only be a single `[[circuit]]` directive in a project, but a circuit function can call other functions.

In case you are compiling C code, the entry point can be marked with `__attribute__((circuit))` directive.

You can link a couple of source files together to create a single circuit.

## Missing type

Following C++ standard types are not currently supported:
* `std::string` (only the C-like strings `char*` are supported)
* `float` , `double` (will be added in an upcoming release)

These types are expected to be added in the future releases. But avoid using them for now. The operations with these types are not circuit-friendly for zkSNARKs/STARKs, so we are working on a special implementation for them.

## Standard library support

We are working on adding support for the standard library. Currently, we only support `std::array` container type and its algorithm. The next one to be added is `std::vector`.
