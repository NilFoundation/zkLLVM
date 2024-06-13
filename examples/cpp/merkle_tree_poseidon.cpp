
#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/poseidon.hpp>

using namespace nil::crypto3;
using namespace nil::crypto3::algebra::curves;

[[circuit]] pallas::base_field_type::value_type merkle_tree_poseidon (
    std::array<typename pallas::base_field_type::value_type, 0x800> layer_100_leaves) {

        // std::array<typename pallas::base_field_type::value_type, 0x400> layer_400_leaves;
        // std::size_t layer_400_size = 0x400;
        // std::array<typename pallas::base_field_type::value_type, 0x200> layer_200_leaves;
        // std::size_t layer_200_size = 0x200;
        // std::array<typename pallas::base_field_type::value_type, 0x100> layer_100_leaves;
        // std::size_t layer_100_size = 0x100;
        std::array<typename pallas::base_field_type::value_type, 0x080> layer_080_leaves;
        std::size_t layer_080_size = 0x080;
        std::array<typename pallas::base_field_type::value_type, 0x040> layer_040_leaves;
        std::size_t layer_040_size = 0x040;
        std::array<typename pallas::base_field_type::value_type, 0x020> layer_020_leaves;
        std::size_t layer_020_size = 0x020;
        std::array<typename pallas::base_field_type::value_type, 0x010> layer_010_leaves;
        std::size_t layer_010_size = 0x010;
        std::array<typename pallas::base_field_type::value_type, 0x008> layer_008_leaves;
        std::size_t layer_008_size = 0x008;
        std::array<typename pallas::base_field_type::value_type, 0x004> layer_004_leaves;
        std::size_t layer_004_size = 0x004;
        std::array<typename pallas::base_field_type::value_type, 0x002> layer_002_leaves;
        std::size_t layer_002_size = 0x002;
        typename pallas::base_field_type::value_type root;


    // for (std::size_t leaf_index = 0; leaf_index < layer_400_size; leaf_index++) {
    //     layer_400_leaves[leaf_index] =
    //         hash<hashes::poseidon>(input[2 * leaf_index], input[2 * leaf_index + 1]);
    // }

    // for (std::size_t leaf_index = 0; leaf_index < layer_200_size; leaf_index++) {
    //     layer_200_leaves[leaf_index] =
    //         hash<hashes::poseidon>(layer_400_leaves[2 * leaf_index], layer_400_leaves[2 * leaf_index + 1]);
    // }

    // for (std::size_t leaf_index = 0; leaf_index < layer_100_size; leaf_index++) {
    //     layer_100_leaves[leaf_index] =
    //         hash<hashes::poseidon>(layer_200_leaves[2 * leaf_index], layer_200_leaves[2 * leaf_index + 1]);
    // }

    for (std::size_t leaf_index = 0; leaf_index < layer_080_size; leaf_index++) {
        layer_080_leaves[leaf_index] =
            hash<hashes::poseidon>(layer_100_leaves[2 * leaf_index], layer_100_leaves[2 * leaf_index + 1]);
    }

    for (std::size_t leaf_index = 0; leaf_index < layer_040_size; leaf_index++) {
        layer_040_leaves[leaf_index] =
            hash<hashes::poseidon>(layer_080_leaves[2 * leaf_index], layer_080_leaves[2 * leaf_index + 1]);
    }

    for (std::size_t leaf_index = 0; leaf_index < layer_020_size; leaf_index++) {
        layer_020_leaves[leaf_index] =
            hash<hashes::poseidon>(layer_040_leaves[2 * leaf_index], layer_040_leaves[2 * leaf_index + 1]);
    }

    for (std::size_t leaf_index = 0; leaf_index < layer_010_size; leaf_index++) {
        layer_010_leaves[leaf_index] =
            hash<hashes::poseidon>(layer_020_leaves[2 * leaf_index], layer_020_leaves[2 * leaf_index + 1]);
    }

    for (std::size_t leaf_index = 0; leaf_index < layer_008_size; leaf_index++) {
        layer_008_leaves[leaf_index] =
            hash<hashes::poseidon>(layer_010_leaves[2 * leaf_index], layer_010_leaves[2 * leaf_index + 1]);
    }

    for (std::size_t leaf_index = 0; leaf_index < layer_004_size; leaf_index++) {
        layer_004_leaves[leaf_index] =
            hash<hashes::poseidon>(layer_008_leaves[2 * leaf_index], layer_008_leaves[2 * leaf_index + 1]);
    }

    for (std::size_t leaf_index = 0; leaf_index < layer_002_size; leaf_index++) {
        layer_002_leaves[leaf_index] =
            hash<hashes::poseidon>(layer_004_leaves[2 * leaf_index], layer_004_leaves[2 * leaf_index + 1]);
    }

    typename pallas::base_field_type::value_type real_root = hash<hashes::poseidon>(layer_002_leaves[0], layer_002_leaves[1]);
    return real_root;
}
