#include <nil/crypto3/algebra/curves/pallas.hpp>

namespace nil {
    namespace crypto3 {
        namespace hashes {
            typedef __attribute__((ext_vector_type(3)))
            typename algebra::curves::pallas::base_field_type::value_type poseidon_block_type;

            poseidon_block_type poseidon(poseidon_block_type input_block);
        }    // namespace hashes
    }        // namespace crypto3
}    // namespace nil

using namespace nil::crypto3;
using namespace nil::crypto3::algebra::curves;

constexpr static const std::size_t input_size = 4;
constexpr static const std::size_t leaves_size = input_size * 2 - 1;

[[circuit]] typename pallas::base_field_type::value_type evaluate_poseidon(
    std::array<typename pallas::base_field_type::value_type, input_size> input) {
    
    std::array<typename pallas::base_field_type::value_type, leaves_size> leaves;
    typename pallas::base_field_type::value_type zero = 0;
    
    typename hashes::poseidon_block_type input_block = {input[0], input[1], input[2]};
    typename hashes::poseidon_block_type hash_result = hashes::poseidon(input_block);

    std::array<typename hashes::poseidon_block_type, input_size> input_blocks;
    std::array<typename hashes::poseidon_block_type, input_size> hash_results;

    for (std::size_t i = 0; i < input_size; i++) {
        input_blocks[i] = {zero, zero, input[i]};
        hash_results[i] = hashes::poseidon(input_blocks[i]);
        leaves[i] = hash_results[i][0];
    }

    return hash_result[0];
}