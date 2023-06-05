---
description: Built in types supported by zkllvm compiler
---

# Builtin types/SDK

Every circuit operates with Finite field elements, and thus, we provide some dialect extensions to standard C++ to make the code more readable. Extensions include custom types for the fields elements. We also provide a set of functions that are highly optimised and can be used when building a circuit.

The full list of the built-in types and functions is as follows.

## Galois field elements
Fields integral types:

* `__zkllvm_field_pallas_base`
* `__zkllvm_field_pallas_scalar`
* `__zkllvm_field_vesta_base`
* `__zkllvm_field_vesta_scalar`
* `__zkllvm_field_bls12381_base`
* `__zkllvm_field_bls12381_scalar`
* `__zkllvm_field_curve25519_base`
* `__zkllvm_field_curve25519_scalar`

Most fields present in to forms - base and scalar. This is due to the fact that we use a lot of elliptic curves cryptography. Every elleptic curve has one base and one scalar field for its operations, and thus, we provide two types of fields (two `pallas` fields for `pallas` curve, two `bls12-381` fields for `bls12-381` curve etc.).

## Built-in functions

* `__builtin_assigner_poseidon_pallas_base` for Poseidon hash function over `pallas` base field
* `__builtin_assigner_sha2_256_pallas_base` for SHA2-256 hash function over `pallas` base field
* `__builtin_assigner_sha2_512_pallas_base` for SHA2-512 hash function over `pallas` base field
* `__builtin_assigner_bls12_optimal_ate_pairing` for BLS12-381 optimal ate pairing
* `__builtin_assigner_zkml_convolution` for zk-ML convolution operation
* `__builtin_assigner_zkml_pooling` for zk-ML pooling operation
* `__builtin_assigner_zkml_ReLU` for zk-ML ReLU activation function
* `__builtin_assigner_zkml_batch_norm` for zk-ML batch normalization operation

### Elliptic curve operations built-ins

Elliptic curve operations implemented in form of built-ins to give more control over the coordinates used by the curve element. Each curve has 4 built-ins for each operation:
* `__builtin_assigner_curve25519_affine_addition` for curve25519 affine addition operation
* `__builtin_assigner_curve25519_affine_subtraction` for curve25519 affine subtraction operation
* `__builtin_assigner_curve25519_affine_double` for curve25519 affine double operation
* `__builtin_assigner_curve25519_affine_scalar_mul` for curve25519 affine scalar multiplication operation