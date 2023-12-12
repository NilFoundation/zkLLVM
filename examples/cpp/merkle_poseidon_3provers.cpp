
#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/poseidon.hpp>

using namespace nil::crypto3;
using namespace nil::crypto3::algebra::curves;


typename pallas::base_field_type::value_type evaluate_root (
    [[private_input]] std::array<typename pallas::base_field_type::value_type, 0x2> layer_0_leaves) {

    typename pallas::base_field_type::value_type real_root = hash<hashes::poseidon>(layer_0_leaves[0], layer_0_leaves[1]);

    return real_root;
}

[[circuit]] bool merkle_tree_poseidon (
    typename pallas::base_field_type::value_type expected_root,
    [[private_input]] std::array<typename pallas::base_field_type::value_type, 0x2> layer_0_leaves1,
    [[private_input]] std::array<typename pallas::base_field_type::value_type, 0x2> layer_0_leaves2
) {

    typename pallas::base_field_type::value_type res1;
    typename pallas::base_field_type::value_type res2;
    typename pallas::base_field_type::value_type res3;

    res1 = evaluate_root(layer_0_leaves1);
#pragma zk_multi_prover 1
    {
        res2 = evaluate_root(layer_0_leaves1);
    }
#pragma zk_multi_prover 2
    {
        res3 =  hash<hashes::poseidon>(res1, res2);
    }

    __builtin_assigner_print_native_pallas_field(res3);

    return (res3 == expected_root);
}
