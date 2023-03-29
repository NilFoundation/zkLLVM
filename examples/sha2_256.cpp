#include <nil/crypto3/algebra/curves/pallas.hpp>

#include <nil/crypto3/hash/algorithms/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>

namespace nil {
    namespace crypto3 {
        namespace hashes {
            typedef __attribute__((ext_vector_type(2)))
            typename algebra::curves::pallas::base_field_type::value_type sha256_block_type;

            sha256_block_type sha256(sha256_block_type first_input_block, sha256_block_type second_input_block);
        }    // namespace hashes
    }        // namespace crypto3
}    // namespace nil

using namespace nil::crypto3;

[[circuit]] typename hashes::sha256_block_type sha256_example(hashes::sha256_block_type first_input_block,
                                                              hashes::sha256_block_type second_input_block) {

    typename hashes::sha256_block_type hash_result = hashes::sha256(first_input_block, second_input_block);

    return hash_result;
}
