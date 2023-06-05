---
description: Examples of zkLLVM programs
---

# Getting Started

In this guide, we will first look at a few examples of circuits which can be compiled with zkLLVM.

## Circuit Examples

In this series of simple examples, we are going to learn how to build a provable computations circuit using the [C++ SDK](https://github.com/NilFoundation/crypto3) and introduce important concepts as we go along.

### Hello World Example

Every provable computations 101 starts with this example, so let's follow the tradition. Every provable computations circuit starts with an entry point function marked with `[[circuit]]` [attribute](https://isocpp.org/wiki/faq/cpp11-language-misc#attributes). The function takes some arguments and returns a result. The function body represents an algorithm, which is going to be compiled into a circuit, which can further be used for proof generation.

{% hint style="danger" %}
There can only be a single \[`[circuit]] directive in a project.`
{% endhint %}

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

Next, let's see the same example but using the `bls12-381` base field type. We will use the interface from the crypto3 library.

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

## Next Steps

If you want to know more about writing circuits, we've prepared a tutorial on how to implement a zk-Bridge using zkLLVM in the [Tutorials](../../tutorials/01-hashes.md) section.

To know more about limitations of the compiler, please see [Limitations](./limitations.md).

If you are looking for more advanced docs, consider checking out our [document](../../architecture/architecture.md) on how to customize the compiler to other proof systems and arithmetizations.