---
description: Examples of zkLLVM programs
---

# Getting Started

In this guide, we will first look at a few examples of circuits which can be compiled with zkLLVM.

## Circuit Examples

In this series of simple examples, we are going to learn how to build a provable computations circuit using the [C++ SDK](https://github.com/NilFoundation/crypto3) and introduce important concepts as we go along.

### 1. Addition

Every provable computations 101 starts with this example, so let's follow the tradition. Every provable computations circuit starts with an entry point function marked with `[[circuit]]` [attribute](https://isocpp.org/wiki/faq/cpp11-language-misc#attributes). The function takes some arguments and returns a result. The function body represents an algorithm, which is going to be compiled into a circuit, which can further be used for proof generation.

{% hint style="danger" %}
There can only be a single \[`[circuit]] directive in a project.`
{% endhint %}

A circuit is comprised of many `components.` A component is any piece of code that can be arithmetized into a circuit.

Standard library integral types, such as `int` , `long` are supported. The standard types  `int32_t` are the modified versions of a fork of the std C++ library, which the compiler includes internally. The user is hidden from this complexity. Some types ex: `strings` are currently not supported (but will soon - see [limitations](../limitations.md)).

```cpp
[[circuit]] std::int32_t addition_std_example(
	std::int32_t a,
	std::int32_t b) {
    auto c = a*b;
    return c;
}
```

The function takes two arguments - two numbers - and multiplies them.

Next, let's see the same example but using the components `bls` from the crypto3 library.

```cpp
#include <nil/crypto3/algebra/curves/bls12.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] typename bls12<381>::base_field_type::value_type addition_example(
	typename bls12<381>::base_field_type::value_type a,
	typename bls12<381>::base_field_type::value_type b) {

    typename bls12<381>::base_field_type::value_type c = a*b;
    return c;
}
```

The most performant algorithms and types will be from the crypto3 SDK. However, the user can still write circuits without including the library, as the compiler provides a C++ dialect with such types supported.

Hence our addition example can be re-written as:

```cpp
[[circuit]] __zkllvm_field_pallas_base addition_example(
	__zkllvm_field_pallas_base a,
	__zkllvm_field_pallas_base b) {

    __zkllvm_field_pallas_base c = a*b;
    return c;
}
```

Please see[ Builtin types](../builtin-types-sdk.md) for more details.

### 2. Galois field arithmetic

In this example, we want to demonstrate the functionality of basic field arithmetic:

```cpp
#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;
typename pallas::base_field_type::value_type square(
    typename pallas::base_field_type::value_type a) {
    return a * a;
}

[[circuit]] typename pallas::base_field_type::value_type field_arithmetic_example(
	typename pallas::base_field_type::value_type a,
	typename pallas::base_field_type::value_type b) {

    typename pallas::base_field_type::value_type c = (a + b)*a + b*(a+b)*(a+b);
    return c*c*c/(b - a) + square(a);
}
```

The function takes two arguments - two numbers - and performs more complex arithmetic operations on them.

### 3. Polygon balance point

Let's try something more difficult. How about the polygon centre of mass calculation? Voila:

```cpp
#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

template <std::size_t VertexAmount>
[[circuit]] std::array<typename pallas::base_field_type::value_type, 2> balance(
    std::array<typename pallas::base_field_type::value_type, 2*VertexAmount> vertexes,
    std::array<typename pallas::base_field_type::value_type, VertexAmount> weights){
    
    std::array<typename pallas::base_field_type::value_type, 2> balance_point;
    typename pallas::base_field_type::value_type balance_weight;

    for (std::size_t VertexIndex = 0; VertexIndex < VertexAmount; VertexIndex++){
        balance_point[0] += vertexes[2*VertexIndex] * weights[VertexIndex];
        balance_point[1] += vertexes[2*VertexIndex + 1] * weights[VertexIndex];
        balance_weight += weights[VertexIndex];
    }

    balance_point[0] /= balance_weight;
    balance_point[1] /= balance_weight;

    return balance_point;
}
```

Balancing a polygon is a well-known problem in computational geometry. The function takes two arguments - an array of vertexes and an array of weights. The function returns a point, which is the centre of mass of the polygon.

### 4. Merkle tree commitment

Psst, you want some cryptography? Then let's build a Merkle tree - a widely used data commitment scheme. Out of curiosity, we will take 2\*\*20 leaves as input, a real-live input size.

{% hint style="info" %}
We support loops! This is unlike other DLS's for writing proof systems.
{% endhint %}

```cpp
#include <nil/crypto3/algebra/curves/pallas.hpp>
#include <nil/crypto3/hash/sha256.hpp>

using namespace nil::crypto3::algebra::curves;
using namespace nil::crypto3;
using namespace nil::marshalling;

[[circuit]] typename hashes::sha256_block_type balance(
    std::array<typename algebra::pallas::value_type, 0x100000> layer_0_leaves){
    
    std::array<typename algebra::pallas::value_type, 0x80000> layer_1_leaves;
    std::array<typename algebra::pallas::value_type, 0x40000> layer_2_leaves;
    std::array<typename algebra::pallas::value_type, 0x20000> layer_3_leaves;
    std::array<typename algebra::pallas::value_type, 0x10000> layer_4_leaves;
    std::array<typename algebra::pallas::value_type, 0x8000> layer_5_leaves;
    std::array<typename algebra::pallas::value_type, 0x4000> layer_6_leaves;
    std::array<typename algebra::pallas::value_type, 0x2000> layer_7_leaves;
    std::array<typename algebra::pallas::value_type, 0x1000> layer_8_leaves;
    std::array<typename algebra::pallas::value_type, 0x800> layer_9_leaves;
    std::array<typename algebra::pallas::value_type, 0x400> layer_10_leaves;
    std::array<typename algebra::pallas::value_type, 0x200> layer_11_leaves;
    std::array<typename algebra::pallas::value_type, 0x100> layer_12_leaves;
    std::array<typename algebra::pallas::value_type, 0x80> layer_13_leaves;
    std::array<typename algebra::pallas::value_type, 0x40> layer_14_leaves;
    std::array<typename algebra::pallas::value_type, 0x20> layer_15_leaves;
    std::array<typename algebra::pallas::value_type, 0x10> layer_16_leaves;
    std::array<typename algebra::pallas::value_type, 0x8> layer_17_leaves;
    std::array<typename algebra::pallas::value_type, 0x4> layer_18_leaves;
    std::array<typename algebra::pallas::value_type, 0x2> layer_19_leaves;
    typename algebra::pallas::value_type root;

    for (std::size_t LeafIndex = 0; LeafIndex < layer_1_leaves.size(); LeafIndex++){
        layer_1_leaves[LeafIndex] = hashes::sha256(layer_0_leaves[2*LeafIndex], layer_0_leaves[2*LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_2_leaves.size(); LeafIndex++){
        layer_2_leaves[LeafIndex] = hashes::sha256(layer_1_leaves[2*LeafIndex], layer_1_leaves[2*LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_3_leaves.size(); LeafIndex++){
        layer_3_leaves[LeafIndex] = hashes::sha256(layer_2_leaves[2*LeafIndex], layer_2_leaves[2*LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_4_leaves.size(); LeafIndex++){
        layer_4_leaves[LeafIndex] = hashes::sha256(layer_3_leaves[2*LeafIndex], layer_3_leaves[2*LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_5_leaves.size(); LeafIndex++){
        layer_5_leaves[LeafIndex] = hashes::sha256(layer_4_leaves[2*LeafIndex], layer_4_leaves[2*LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_6_leaves.size(); LeafIndex++){
        layer_6_leaves[LeafIndex] = hashes::sha256(layer_5_leaves[2*LeafIndex], layer_5_leaves[2*LeafIndex + 1]);
    }
    
    for (std::size_t LeafIndex = 0; LeafIndex < layer_7_leaves.size(); LeafIndex++){
        layer_7_leaves[LeafIndex] = hashes::sha256(layer_6_leaves[2*LeafIndex], layer_6_leaves[2*LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_8_leaves.size(); LeafIndex++){
        layer_8_leaves[LeafIndex] = hashes::sha256(layer_7_leaves[2*LeafIndex], layer_7_leaves[2*LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_9_leaves.size(); LeafIndex++){
        layer_9_leaves[LeafIndex] = hashes::sha256(layer_8_leaves[2*LeafIndex], layer_8_leaves[2*LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_10_leaves.size(); LeafIndex++){
        layer_10_leaves[LeafIndex] = hashes::sha256(layer_9_leaves[2*LeafIndex], layer_9_leaves[2*LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_11_leaves.size(); LeafIndex++){
        layer_11_leaves[LeafIndex] = hashes::sha256(layer_10_leaves[2*LeafIndex], layer_10_leaves[2*LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_12_leaves.size(); LeafIndex++){
        layer_12_leaves[LeafIndex] = hashes::sha256(layer_11_leaves[2*LeafIndex], layer_11_leaves[2*LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_13_leaves.size(); LeafIndex++){
        layer_13_leaves[LeafIndex] = hashes::sha256(layer_12_leaves[2*LeafIndex], layer_12_leaves[2*LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_14_leaves.size(); LeafIndex++){
        layer_14_leaves[LeafIndex] = hashes::sha256(layer_13_leaves[2*LeafIndex], layer_13_leaves[2*LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_15_leaves.size(); LeafIndex++){
        layer_15_leaves[LeafIndex] = hashes::sha256(layer_14_leaves[2*LeafIndex], layer_14_leaves[2*LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_16_leaves.size(); LeafIndex++){
        layer_16_leaves[LeafIndex] = hashes::sha256(layer_15_leaves[2*LeafIndex], layer_15_leaves[2*LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_17_leaves.size(); LeafIndex++){
        layer_17_leaves[LeafIndex] = hashes::sha256(layer_16_leaves[2*LeafIndex], layer_16_leaves[2*LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_18_leaves.size(); LeafIndex++){
        layer_18_leaves[LeafIndex] = hashes::sha256(layer_17_leaves[2*LeafIndex], layer_17_leaves[2*LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_19_leaves.size(); LeafIndex++){
        layer_19_leaves[LeafIndex] = hashes::sha256(layer_18_leaves[2*LeafIndex], layer_18_leaves[2*LeafIndex + 1]);
    }

    root = hashes::sha256(layer_19_leaves[0], layer_19_leaves[1]);

    return root;
}
```

Awesome, we just built a circuit of the world's most used commitment scheme.

## Next Steps

Now, let's see how we can compile these examples to generate a circuit representation.
