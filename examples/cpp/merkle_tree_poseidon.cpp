
#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/poseidon.hpp>

using namespace nil::crypto3;
using namespace nil::crypto3::algebra::curves;

[[circuit]] pallas::base_field_type::value_type merkle_tree_poseidon (
    std::array<typename pallas::base_field_type::value_type, 0x20> layer_0_leaves) {
        
        std::array<typename pallas::base_field_type::value_type, 0x10> layer_1_leaves;
        std::size_t layer_1_size = 0x10;
        std::array<typename pallas::base_field_type::value_type, 0x8> layer_2_leaves;
        std::size_t layer_2_size = 0x8;
        std::array<typename pallas::base_field_type::value_type, 0x4> layer_3_leaves;
        std::size_t layer_3_size = 0x4;
        std::array<typename pallas::base_field_type::value_type, 0x2> layer_4_leaves;
        std::size_t layer_4_size = 0x2;
        typename pallas::base_field_type::value_type root;

        for (std::size_t leaf_index = 0; leaf_index < layer_1_size; leaf_index++) {
        layer_1_leaves[leaf_index] =
            hash<hashes::poseidon>(layer_0_leaves[2 * leaf_index], layer_0_leaves[2 * leaf_index + 1]);
    }

    for (std::size_t leaf_index = 0; leaf_index < layer_2_size; leaf_index++) {
        layer_2_leaves[leaf_index] =
            hash<hashes::poseidon>(layer_1_leaves[2 * leaf_index], layer_1_leaves[2 * leaf_index + 1]);
    }

    for (std::size_t leaf_index = 0; leaf_index < layer_3_size; leaf_index++) {
        layer_3_leaves[leaf_index] =
            hash<hashes::poseidon>(layer_2_leaves[2 * leaf_index], layer_2_leaves[2 * leaf_index + 1]);
    }

    for (std::size_t leaf_index = 0; leaf_index < layer_4_size; leaf_index++) {
        layer_4_leaves[leaf_index] =
            hash<hashes::poseidon>(layer_3_leaves[2 * leaf_index], layer_3_leaves[2 * leaf_index + 1]);
    }

    root = hash<hashes::poseidon>(layer_4_leaves[0], layer_4_leaves[1]);

    return root;
}
