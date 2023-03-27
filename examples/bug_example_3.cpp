#include <nil/crypto3/algebra/curves/pallas.hpp>

namespace nil{
    namespace crypto3{
        namespace hashes {
            typedef __attribute__((ext_vector_type(2))) typename algebra::curves::pallas::base_field_type::value_type sha256_block_type;

            sha256_block_type sha256(sha256_block_type first_input_block, sha256_block_type second_input_block);
        }
    }
}

// constexpr static const std::size_t input_log2 = 2;
constexpr static const std::size_t input_size = 4;
constexpr static const std::size_t leaves_size = 7;
// constexpr static const std::size_t input_size = 1 << input_log2;
// constexpr static const std::size_t leaves_size = 2 * input_size - 1;


using namespace nil::crypto3;

[[circuit]] typename hashes::sha256_block_type sha256_example(
    std::array<typename hashes::sha256_block_type, input_size> input) {

    std::array<typename hashes::sha256_block_type, leaves_size> leaves;

    for (std::size_t i = 0; i < input_size; i++) {
        leaves[i] = input[i];
    }

    int LeafIter = 0;
    for (std::size_t i = 0; i < (input_size-1) * 2; i += 2) {
        leaves[input_size + LeafIter] = hashes::sha256(leaves[i], leaves[i + 1]);
        LeafIter += 1;
    }

    return leaves[leaves_size - 1];
}