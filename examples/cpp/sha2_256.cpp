#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>

using namespace nil::crypto3;

[[circuit]] typename hashes::sha2<256>::block_type
    sha256_example(typename hashes::sha2<256>::block_type first_input_block,
                   typename hashes::sha2<256>::block_type second_input_block) {

    typename hashes::sha2<256>::block_type hash_result = hash<hashes::sha2<256>>(first_input_block, second_input_block);

    return hash_result;
}
