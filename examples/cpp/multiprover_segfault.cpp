
#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/poseidon.hpp>

using namespace nil::crypto3;
using namespace nil::crypto3::algebra::curves;


[[circuit]] bool check_root (
    typename pallas::base_field_type::value_type expected_root,
    [[private_input]] std::array<typename pallas::base_field_type::value_type, 0x8> layer_0_leaves) {

        std::array<typename pallas::base_field_type::value_type, 0x4> layer_1_leaves;
        std::size_t layer_1_size = 0x4;
        std::array<typename pallas::base_field_type::value_type, 0x2> layer_2_leaves;
        std::size_t layer_2_size = 0x2;
        typename pallas::base_field_type::value_type real_root;


    for (std::size_t leaf_index = 0; leaf_index < layer_1_size; leaf_index++) {
        layer_1_leaves[leaf_index] =
            hash<hashes::poseidon>(layer_0_leaves[2 * leaf_index], layer_0_leaves[2 * leaf_index + 1]);
    }

#pragma zk_multi_prover 1
    {
        for (std::size_t leaf_index = 0; leaf_index < layer_2_size; leaf_index++) {
            layer_2_leaves[leaf_index] =
                hash<hashes::poseidon>(layer_1_leaves[2 * leaf_index], layer_1_leaves[2 * leaf_index + 1]);
        }
    }

#pragma zk_multi_prover 2
    {
        real_root = hash<hashes::poseidon>(layer_2_leaves[0], layer_2_leaves[1]);
    }

    bool res = (real_root == expected_root);
    // __builtin_assigner_exit_check(res);

    return res;
}
