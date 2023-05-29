# Merkle tree commitment scheme

Merkle tree is often used in blockchain to commit to a set of data. In this tutorial, we will show how to construct a Merkle tree verification circuit with zkLLVM.

First of all, let's define the structure of a Merkle tree. A Merkle tree is a binary tree, where each leaf node contains a hash of a data block and each non-leaf node contains a hash of its children. The root of the tree is called a Merkle root and it contains a hash of all the data in the tree.

As for the [first tutorial](01-hashes.md) of this serie, we will use `sha2-256` hash function as and `std::array` as a container for data blocks. We include the same headers as in the first tutorial and use the same namespace.

{% hint style="info" %}
zkLLVM supports `sha2-256`, `sha2-512` and `Poseidon` hash functions.
{% endhint %}

{% hint style="info" %}
We use `std::array` as a container for data blocks, but we understand the necessity of supporting other containers. We are working on it. Extended support of `std` algorithms is one of our priorities.
{% endhint %}

```cpp
#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>

using namespace nil::crypto3;
```

Now let's add the first layer hashing function. It takes pairs of leaves and hashes them together. The resulting array of hashes will be two times smaller than the original array of leaves.

```cpp
#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>

using namespace nil::crypto3;

[[circuit]] typename hashes::sha2<256>::block_type
    hash_layer_1(std::array<typename hashes::sha2<256>::block_type, 0x10> layer_0_leaves) {

    std::array<typename hashes::sha2<256>::block_type, 0x8> layer_1_leaves;

    for (std::size_t leaf_index = 0; leaf_index < layer_1_leaves.size(); leaf_index++) {
        layer_1_leaves[leaf_index] =
            hash<hashes::sha2<256>>(layer_0_leaves[2 * leaf_index], layer_0_leaves[2 * leaf_index + 1]);
    }

    return layer_1_leaves;
}
```

Every next layer is constructed in the same way. The only difference is that the size of the array of leaves is two times smaller than the previous one. The last layer will contain only one element, which is the Merkle root. We will return it from the function. So, let's add the rest logic and finish the circuit.

```cpp

#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>

using namespace nil::crypto3;

[[circuit]] typename hashes::sha2<256>::block_type
    balance(std::array<typename hashes::sha2<256>::block_type, 0x10> layer_0_leaves) {

    std::array<typename hashes::sha2<256>::block_type, 0x8> layer_1_leaves;
    std::size_t layer_1_size = 0x8;
    std::array<typename hashes::sha2<256>::block_type, 0x4> layer_2_leaves;
    std::size_t layer_2_size = 0x4;
    std::array<typename hashes::sha2<256>::block_type, 0x2> layer_3_leaves;
    std::size_t layer_3_size = 0x2;
    typename hashes::sha2<256>::block_type root;

    for (std::size_t leaf_index = 0; leaf_index < layer_1_size; leaf_index++) {
        layer_1_leaves[leaf_index] =
            hash<hashes::sha2<256>>(layer_0_leaves[2 * leaf_index], layer_0_leaves[2 * leaf_index + 1]);
    }

    for (std::size_t leaf_index = 0; leaf_index < layer_2_size; leaf_index++) {
        layer_2_leaves[leaf_index] =
            hash<hashes::sha2<256>>(layer_1_leaves[2 * leaf_index], layer_1_leaves[2 * leaf_index + 1]);
    }

    for (std::size_t leaf_index = 0; leaf_index < layer_3_size; leaf_index++) {
        layer_3_leaves[leaf_index] =
            hash<hashes::sha2<256>>(layer_2_leaves[2 * leaf_index], layer_2_leaves[2 * leaf_index + 1]);
    }

    root = hash<hashes::sha2<256>>(layer_3_leaves[0], layer_3_leaves[1]);

    return root;
}
```
{% hint style="info" %}
Merkle tree is a very common data structure. In this tutorial you've learn how to construct it from scratch. Though, you don't need to do it every time. We have Merkle Tree algorithm in our Crypto3 library of optimized circuit-friendly algorithms.
{% endhint %}

We will need this Merkle Tree circuit in the next tutorial when build a zkBridge.
