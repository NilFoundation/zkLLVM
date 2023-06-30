# First circuit with hashes

Circuit development slightly differs from the usual software development. The main difference is that you only can use pure functions in your circuits. We will provide other recommendations for efficient circuit development in this tutorial and the following ones.

The good news is that many circuit-friendly algorithm implementations already became part of the SDK.
You can use them in your circuits, speeding up the development process.
In this tutorial we will show you how to use hashes in your circuits — in particular, the `sha2-256` hash function.

{% hint style="info" %}
zkLLVM SDK has `sha2-256`, `sha2-512` and `Poseidon` hash functions.
Later we are going to add more hash functions, such as `Pedersen` hash and `keccak` (`sha3`).
{% endhint %}

To use hashes in your C++ code, include the following header:

```cpp
#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>
```

Then you can use the `hash` function to calculate hashes of the given data.
The function can work with different forms of input.
It takes one template parameter — the hash algorithm that you want to use.
In this example, we will use the `sha2-256` hash function with two `sha2-256` blocks input. 

```cpp
#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>

using namespace nil::crypto3::hashes;

[[circuit]] typename sha2<256>::block_type sha256_example(
    typename sha2<256>::block_type first_input_block,
    typename sha2<256>::block_type second_input_block) {
    
    typename sha2<256>::block_type hash_result = 
        hash<sha2<256>>(first_input_block, second_input_block);
    return hash_result;
}
```

We use namespace `nil::crypto3::hashes` to avoid writing `nil::crypto3::hashes::sha2<256>` every time we want to use `sha2-256` hash function.
Instead, we can write `sha2<256>`.

It's recommended to use fixed-size containers, since they reduce the number of constraints in the resulting circuit.
In this example we use `std::array` with 64 elements.
We are constantly working on optimizing `std` algorithms implementations for circuit form, so other containers will be available soon.

Let's hash elements of the array one by one and return the result:

```cpp
#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>

using namespace nil::crypto3::hashes;

[[circuit]] typename sha2<256>::block_type sha256_example( std::array<block_data_type, 64> input_blocks) {
    
    typename sha2<256>::block_type result = input_blocks[0];
    for (int i = 1; i < input_blocks.size(); i++) {
        result = hash<sha2<256>>(result, input_blocks[i]);
    }

    return result;
}
```

Now a real-life example.
Let's assume that we have a blockchain with blocks having the following simplified structure:

```cpp
struct block_data_type {
    typename hashes::sha2<256>::block_type prev_block_hash;
    typename hashes::sha2<256>::block_type data;
};
```

Let's imagine that we want to verify that a particular block was created correctly.
We have the hash of a previous block, which is already confirmed, and all the blocks from that verified block to the one we want to verify.
For each unverified block, we will calculate the hash of its parent and
check that it matches the hash stored in the block.

We will start with a simple example where 
Then we need to check that the hash of the previous block in the chain is equal to the hash of the previous block in the chain in the block we want to verify.


```cpp
#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>

using namespace nil::crypto3::hashes;

struct block_data_type {
    typename sha2<256>::block_type prev_block_hash;
    typename sha2<256>::block_type data;
};

bool is_same(
    typename hashes::sha2<256>::block_type block0,
    typename hashes::sha2<256>::block_type block1) {

    return block0[0] == block1[0] && block0[1] == block1[1];
}

[[circuit]] bool verify_protocol_state_proof (
    typename sha2<256>::block_type last_confirmed_block_hash,
    block_data_type unconfirmed_block) {
    
    return is_same(
        unconfirmed_block.prev_block_hash,
        last_confirmed_block_hash);
}
```

Here we use a simple function `is_same` to compare two hashes.
You can call any other functions in your circuits without overhead, but they should be pure functions.

Now let's do the same process but with a list of blocks instead of a single block:

```cpp
#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>

using namespace nil::crypto3::hashes;

struct block_data_type {
    typename sha2<256>::block_type prev_block_hash;
    typename sha2<256>::block_type data;
};

bool is_same(
    typename hashes::sha2<256>::block_type block0,
    typename hashes::sha2<256>::block_type block1) {

    return block0[0] == block1[0] && block0[1] == block1[1];
}

[[circuit]] bool verify_protocol_state_proof (
    typename sha2<256>::block_type last_confirmed_block_hash,
    std::array<block_data_type, 64> unconfirmed_blocks) {
    
    for (int i = 0; i < unconfirmed_blocks.size(); i++) {
        
        // Check hashes correctness
        if (i == 0) {
            if (!is_same(
                unconfirmed_blocks[i].prev_block_hash,
                last_confirmed_block_hash)) {
                return false;
            }
        } else {
            typename sha2<256>::block_type evaluated_block_hash =
                hash<sha2<256>>(
                    unconfirmed_blocks[i-1].prev_block_hash,
                    unconfirmed_blocks[i-1].data);
            
            if (!is_same(
                unconfirmed_blocks[i].prev_block_hash,
                evaluated_block_hash)) {
                return false;
            }
        }
    }
    return true;
}
```

Congratulations!
You have just created your first circuit using hashes.
We will use it later to verify blocks in the protocol state.