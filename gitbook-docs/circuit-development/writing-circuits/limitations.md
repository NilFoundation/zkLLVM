---
description: Current limitations of the compiler
---

# Limitations

The zkLLVM project is under active development. The current limitations are listed below and will be addressed/updated in future versions.

We are hardly working on removing these limitations, but for now, please keep them in mind when writing circuits. Our final goal is to make the zkLLVM compiler as close to standard C++ as possible.

## Entry point

The entry point of the project must be marked with `[[circuit]]` directive.
There can only be a single `[[circuit]]` directive in a project, but a circuit function can call other functions.

You can link a couple of source files together to create a single circuit.

## Missing type

Following C++ standard types are not supported:
* `string`
* `float` , `double` (will be added in an upcoming release)

## Standard library support

We are working on adding support for the standard library. Currently, we only support `std::array` container type and its algorithm. The next one to be added is `std::vector`.
