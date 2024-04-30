
#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/poseidon.hpp>

using namespace nil::crypto3;
using namespace nil::crypto3::algebra::curves;

typename pallas::base_field_type::value_type
    evaluate_root(std::array<typename pallas::base_field_type::value_type, 0x4> layer_0_leaves) {

    std::array<typename pallas::base_field_type::value_type, 0x2> layer_1_leaves;
    std::size_t layer_1_size = 0x2;

    for (std::size_t leaf_index = 0; leaf_index < layer_1_size; leaf_index++) {
        layer_1_leaves[leaf_index] =
            hash<hashes::poseidon>(layer_0_leaves[2 * leaf_index], layer_0_leaves[2 * leaf_index + 1]);
    }

    typename pallas::base_field_type::value_type real_root =
        hash<hashes::poseidon>(layer_1_leaves[0], layer_1_leaves[1]);

    return real_root;
}

[[circuit]] bool merkle_tree_poseidon(typename pallas::base_field_type::value_type expected_root,
                                      [[private_input]] std::array<typename pallas::base_field_type::value_type, 0x4>
                                          layer_0_leaves0,
                                      [[private_input]] std::array<typename pallas::base_field_type::value_type, 0x4>
                                          layer_0_leaves1,
                                      [[private_input]] std::array<typename pallas::base_field_type::value_type, 0x4>
                                          layer_0_leaves2,
                                      [[private_input]] std::array<typename pallas::base_field_type::value_type, 0x4>
                                          layer_0_leaves3) {

    typename pallas::base_field_type::value_type res0;
    typename pallas::base_field_type::value_type res1;
    typename pallas::base_field_type::value_type res2;
    typename pallas::base_field_type::value_type res3;
    typename pallas::base_field_type::value_type real_root;

    res0 = evaluate_root(layer_0_leaves0);
#pragma zk_multi_prover 1
    { res1 = evaluate_root(layer_0_leaves1); }
#pragma zk_multi_prover 2
    { res2 = evaluate_root(layer_0_leaves2); }
#pragma zk_multi_prover 3
    { res3 = evaluate_root(layer_0_leaves3); }

#pragma zk_multi_prover 4
    { real_root = evaluate_root({res0, res1, res2, res3}); }

    bool output = (real_root == expected_root);
    __builtin_assigner_exit_check(output);
    return output;
}
