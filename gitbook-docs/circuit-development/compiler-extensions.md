---
description: Built in types supported by zkllvm compiler
---

# Circuit-specific compiler extensions

Many circuit-friendly algorithms require custom types and functions. We provide a set of them in the SDK. You can use them in your circuits.

## Builtin types/SDK

Every circuit operates with Finite field elements, and thus, we provide some dialect extensions to standard C++ to make the code more readable. Extensions include custom types for the fields elements. We also provide a set of functions that are highly optimised and can be used when building a circuit.

The full list of the built-in types and functions is as follows.

### Galois field elements
Fields integral types:

* `__zkllvm_field_pallas_base`
* `__zkllvm_field_pallas_scalar`
* `__zkllvm_field_vesta_base`
* `__zkllvm_field_vesta_scalar`
* `__zkllvm_field_bls12381_base`
* `__zkllvm_field_bls12381_scalar`
* `__zkllvm_field_curve25519_base`
* `__zkllvm_field_curve25519_scalar`

Every field presents in two forms - base and scalar. This is due to the fact that we use a lot of elliptic curves cryptography. Every elleptic curve has one base and one scalar field for its operations, and thus, we provide two types of fields (two `pallas` fields for `pallas` curve, two `bls12-381` fields for `bls12-381` curve etc.).

Code example for pallas curve:

```cpp
#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;
typename pallas::base_field_type::value_type pow(typename pallas::base_field_type::value_type a, int n) {
    if (n == 0)
        return 1;

    typename pallas::base_field_type::value_type res = 1;
    for (int i = 0; i < n; ++i) {
        res *= a;
    }
    return res;
}

[[circuit]] typename pallas::base_field_type::value_type
    field_arithmetic_example(typename pallas::base_field_type::value_type a,
                             typename pallas::base_field_type::value_type b) {

    typename pallas::base_field_type::value_type c = (a + b) * a + b * (a + b) * (a + b);
    const typename pallas::base_field_type::value_type constant = 0x12345678901234567890_cppui255;
    return c * c * c / (b - a) + pow(a, 2) + constant;
}
``

## Built-in functions

### SHA2-256 built-in hash function

Apart from a general case sha2-256 hash funciton, we have a circuit-friendly implementation of it. It takes 

* Input: two blocks, each block is packed in two `pallas` field elements.
* Output: one block packed in two `pallas` field elements.

Code example:

```cpp
#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>

using namespace nil::crypto3::hashes;

struct block_data_type {
    typename hashes::sha2<256>::block_type prev_block_hash;
    typename hashes::sha2<256>::block_type data;
};

[[circuit]] typename sha2<256>::block_type sha256_example(
        std::array<block_data_type, 64> input_blocks) {
    
    typename sha2<256>::block_type result = input_blocks[0];
    for (int i = 1; i < input_blocks.size(); i++) {
        result = hash<sha2<256>>(result, input_blocks[i]);
    }

    return result;
}

```

### SHA2-512 built-in hash function

This function is also implemented in SDK and also has an optimized circuit version. This optimized version was designed to being used as part of EDDSA signature algorithm, so it has tricky interface.

* Input: one `eddsa` curve group element (point) `R`, one `eddsa` curve group element (point) representing the public key `pk` and one message block `M`.
* Output: one `eddsa` scalar field element

Code example:

```cpp
#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>

using namespace nil::crypto3;
using namespace nil::crypto3::algebra::curves;

typedef __attribute__((ext_vector_type(4)))
                typename pallas::base_field_type::value_type eddsa_message_block_type;


[[circuit]] __zkllvm_field_curve25519_scalar verify_eddsa_signature (
    __zkllvm_curve_curve25519 R, 
    __zkllvm_curve_curve25519 pk, 
    eddsa_message_block_type M
    ) {

        __zkllvm_field_curve25519_scalar k = __builtin_assigner_sha2_512_curve25519(R, pk, M);

        return k;
}
```

### Bit de/composition built-in functions

Circuit algorithms usually operate with Galois field elements. (Read more about efficient circuit development in the [corresponding section](optimizations.md). But since sometimes you need to operate with bits representation, we have dedicated functions for that. They can be used, for example, to serialize/deserialize sha2-256 input and output.

#### Bit composition:

* Input: pointer to input data, number of bits to compose, bit order mode (MSB or LSB).
* Output: one `pallas` field element.

Code example is below. Bits are composed in MSB (most significant bit) order and stored in pallas elements for circuit efficiency.

```cpp

include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

constexpr bool is_msb = false;

[[circuit]] typename pallas::base_field_type::value_type compose(
    std::array<typename pallas::base_field_type::value_type, 128> input) {

    return __builtin_assigner_bit_composition(input.data(), 128, is_msb);
}

```

#### Bit decomposition:

* Input: pointer to ouptut  data, number of bits to compose, one `pallas` field element and bit order mode (MSB or LSB).
* Output: no output, result is being written to the pointer.

Code example is below. Bits are composed in MSB (most significant bit) order and stored in pallas elements for circuit efficiency.

```cpp

#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

constexpr bool is_msb = true;
constexpr std::size_t bits_amount = 64;

[[circuit]] std::array<typename pallas::base_field_type::value_type, bits_amount>
        decompose(uint64_t input) {

    std::array<typename pallas::base_field_type::value_type, bits_amount> result;

    __builtin_assigner_bit_decomposition(result.data(), bits_amount, input, is_msb);

    return result;
}

```

### Enforced check built-in function

Sometimes you need to enforce some condition in the middle of the circuit. For example, you need to check that the signature is valid. If it is not, you need the circuit to be stopped and the proof to be rejected. This can be done with the `__builtin_assigner_exit_check` function.

* Input: one `bool`.
* Output: no output.

Code example:

```cpp
[[circuit]] void exit_check(bool condition) {
    __builtin_assigner_exit_check(condition);
}
```