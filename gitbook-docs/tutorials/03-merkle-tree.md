# Merkle tree commitment scheme

Merkle tree is often used in blockchain to commit to a set of data.
This tutorial will show constructing a Merkle tree verification circuit with zkLLVM.

First of all, let's define the structure of a Merkle tree.
It is a binary tree where each leaf node contains a hash of a data block,
and each non-leaf node contains a hash of its children.
The tree's root element is called a Merkle root.
It contains a hash of all the data in the tree.

Like in the [first tutorial](01-hashes.md) of this series, we will use the `sha2-256` hash function
and `std::array` as a container for data blocks.
We include the same headers as in the first tutorial and use the same namespace.

{% hint style="info" %}
zkLLVM supports `sha2-256`, `sha2-512`, and `Poseidon` hash functions.
{% endhint %}

{% hint style="info" %}
In this tutorial we use `std::array` as a container for data blocks,
but we understand the necessity of supporting other containers.
We are working on it.
Extended support of `std` algorithms is one of our priorities.
{% endhint %}

```cpp
#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>

using namespace nil::crypto3::hashes;
```

Now let's add the first layer hashing function.
It takes pairs of leaves and hashes them together.
The resulting array of hashes will be two times smaller than the original array of leaves.

```cpp
#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>

using namespace nil::crypto3::hashes;

[[circuit]] typename sha2<256>::block_type
    hash_layer_1(std::array<typename sha2<256>::block_type, 0x10> layer_0_leaves) {

    std::array<typename sha2<256>::block_type, 0x8> layer_1_leaves;

    for (std::size_t leaf_index = 0; leaf_index < layer_1_leaves.size(); leaf_index++) {
        layer_1_leaves[leaf_index] =
            hash<sha2<256>>(layer_0_leaves[2 * leaf_index], layer_0_leaves[2 * leaf_index + 1]);
    }

    return layer_1_leaves;
}
```

Each subsequent layer is constructed in the same way.
The only difference is that the array of leaves is two times smaller than the previous one.
The last layer will contain only one element: the Merkle root.
The circuit function that we want to build should return this element.

So, let's add the remaining logic and finish the circuit:

```cpp
#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>

using namespace nil::crypto3::hashes;

[[circuit]] typename sha2<256>::block_type
    balance(std::array<typename sha2<256>::block_type, 0x10> layer_0_leaves) {

    std::array<typename sha2<256>::block_type, 0x8> layer_1_leaves;
    std::size_t layer_1_size = 0x8;
    std::array<typename sha2<256>::block_type, 0x4> layer_2_leaves;
    std::size_t layer_2_size = 0x4;
    std::array<typename sha2<256>::block_type, 0x2> layer_3_leaves;
    std::size_t layer_3_size = 0x2;
    typename sha2<256>::block_type root;

    for (std::size_t leaf_index = 0; leaf_index < layer_1_size; leaf_index++) {
        layer_1_leaves[leaf_index] =
            hash<sha2<256>>(layer_0_leaves[2 * leaf_index], layer_0_leaves[2 * leaf_index + 1]);
    }

    for (std::size_t leaf_index = 0; leaf_index < layer_2_size; leaf_index++) {
        layer_2_leaves[leaf_index] =
            hash<sha2<256>>(layer_1_leaves[2 * leaf_index], layer_1_leaves[2 * leaf_index + 1]);
    }

    for (std::size_t leaf_index = 0; leaf_index < layer_3_size; leaf_index++) {
        layer_3_leaves[leaf_index] =
            hash<sha2<256>>(layer_2_leaves[2 * leaf_index], layer_2_leaves[2 * leaf_index + 1]);
    }

    root = hash<sha2<256>>(layer_3_leaves[0], layer_3_leaves[1]);

    return root;
}
```

{% hint style="info" %}
Merkle tree is a widely-used data structure.
In this tutorial, you've learned how to construct it from scratch.
Though, you don't need to do it every time.
Crypto3 library has an optimized circuit-friendly implementation of Merkle tree and other algorithms.
{% endhint %}

We will use this Merkle Tree circuit to build a zkBridge in the next part of the tutorial.
