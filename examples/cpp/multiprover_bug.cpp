
#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/poseidon.hpp>

using namespace nil::crypto3;
using namespace nil::crypto3::algebra::curves;


bool check_root (
    typename pallas::base_field_type::value_type expected_root,
    [[private_input]] std::array<typename pallas::base_field_type::value_type, 0x2> layer_0_leaves) {

    typename pallas::base_field_type::value_type real_root = hash<hashes::poseidon>(layer_0_leaves[0], layer_0_leaves[1]);

    return (real_root == expected_root);
}

[[circuit]] bool merkle_tree_poseidon (
    typename pallas::base_field_type::value_type expected_root1,
    [[private_input]] std::array<typename pallas::base_field_type::value_type, 0x2> layer_0_leaves1,
    typename pallas::base_field_type::value_type expected_root2,
    [[private_input]] std::array<typename pallas::base_field_type::value_type, 0x2> layer_0_leaves2
) {

    bool res1 = check_root(expected_root1, layer_0_leaves1);

    bool res2;

#pragma zk_multi_prover 1
    {
        res2 = check_root(expected_root2, layer_0_leaves2);
    }

    return res1 & res2;
}
