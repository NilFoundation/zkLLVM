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

typename hashes::poseidon_block_type evaluate_poseidon(typename pallas::base_field_type::value_type a,
                                                        typename pallas::base_field_type::value_type b,
                                                        typename pallas::base_field_type::value_type c) {
    typename hashes::poseidon_block_type input_block = {a, b, c};
    typename hashes::poseidon_block_type hash_result = hashes::poseidon(input_block);
    return hash_result;
}

typename pallas::base_field_type::value_type poseidon_two_inputs(typename pallas::base_field_type::value_type a,
                                                                   typename pallas::base_field_type::value_type b) {
    
    typename pallas::base_field_type::value_type zero = 0;
    typename hashes::poseidon_block_type hash_result = evaluate_poseidon(zero, a, b);
    return hash_result[2];
}

typename pallas::base_field_type::value_type poseidon_one_input(typename pallas::base_field_type::value_type a) {
    typename pallas::base_field_type::value_type zero = 0;
    typename hashes::poseidon_block_type hash_result = evaluate_poseidon(zero, zero, a);
    return hash_result[2];
}


constexpr static const std::size_t input_size = 4;
constexpr static const std::size_t leaves_size = input_size * 2 - 1;

[[circuit]] pallas::base_field_type::value_type merkle_tree_poseidon (
    std::array<typename     algebra::curves::pallas::base_field_type::value_type, input_size> input) {
        std::array<typename algebra::curves::pallas::base_field_type::value_type, leaves_size> leaves;

        for (std::size_t i = 0; i < input_size; i++) {
            leaves[i] = poseidon_one_input(input[i]);
        }

        for (std::size_t i = 0; i < input_size-1; i++) {
            leaves[input_size + i] = poseidon_two_inputs(leaves[2*i], leaves[2*i + 1]);
        }

        return leaves[leaves_size -1];
    }
