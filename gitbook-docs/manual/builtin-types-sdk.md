---
description: Built in types supported by zkllvm compiler
---

# Builtin types/SDK

Every PLONK circuit operates with the Finite fields elements and thus we provide some dialect extension to standard C++ to make the code more readable. Such extension includes custom types for the fields and elliptic curves elements. We also provide a set of functions that are highly optimised and can be used when building a circuit.

Full list of the builtin types and functions is as follows.

Fields integral types:

* `__zkllvm_field_pallas_base`
* `__zkllvm_field_pallas_scalar`
* `__zkllvm_field_vesta_base`
* `__zkllvm_field_vesta_scalar`
* `__zkllvm_field_bls12381_base`
* `__zkllvm_field_bls12381_scalar`
* `__zkllvm_field_curve25519_base`
* `__zkllvm_field_curve25519_scalar`

Curves integral types:

* `__zkllvm_curve_pallas`
* `__zkllvm_curve_vesta`
* `__zkllvm_curve_bls12381`
* `__zkllvm_curve_curve25519`
