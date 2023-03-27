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

typename pallas::base_field_type::value_type evaluate_poseidon(typename pallas::base_field_type::value_type a,
                                                        typename pallas::base_field_type::value_type b,
                                                        typename pallas::base_field_type::value_type c) {
    typename hashes::poseidon_block_type input_block = {a, b, c};
    typename hashes::poseidon_block_type hash_result = hashes::poseidon(input_block);
    return hash_result[2];
}

constexpr static const std::size_t input_log2 = 5;
constexpr static const std::size_t input_size = 1 << input_log2;
constexpr static const std::size_t leaves_size = input_size * 2 - 1;

[[circuit]] pallas::base_field_type::value_type merkle_tree_poseidon (
    std::array<typename     algebra::curves::pallas::base_field_type::value_type, input_size> input) {
        std::array<typename algebra::curves::pallas::base_field_type::value_type, leaves_size> leaves;

        typename pallas::base_field_type::value_type zero = 0;

        for (std::size_t i = 0; i < input_size; i++) {
            leaves[i] = evaluate_poseidon(zero, input[i], zero);
        }

        for (std::size_t i = 0; i < input_size-1; i++) {
            leaves[input_size + i] = evaluate_poseidon(zero, leaves[i+i], leaves[i+i + 1]);
        }

        return leaves[leaves_size -1];
    }
