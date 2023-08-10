#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>
#include <cstdint>

using namespace nil::crypto3;

constexpr std::size_t validators_amount_log2 = 6;
constexpr std::size_t validators_per_leaf_log2 = 2;

const std::array<typename hashes::sha2<256>::block_type, 39> precomputed_zero_hashes = {{
{0xf5a5fd42d16a20302798ef6ed309979b_cppui255, 0x43003d2320d9f0e8ea9831a92759fb4b_cppui255},
{0xdb56114e00fdd4c1f85c892bf35ac9a8_cppui255, 0x9289aaecb1ebd0a96cde606a748b5d71_cppui255},
{0xc78009fdf07fc56a11f122370658a353_cppui255, 0xaaa542ed63e44c4bc15ff4cd105ab33c_cppui255},
{0x536d98837f2dd165a55d5eeae9148595_cppui255, 0x4472d56f246df256bf3cae19352a123c_cppui255},
{0x9efde052aa15429fae05bad4d0b1d7c6_cppui255, 0x4da64d03d7a1854a588c2cb8430c0d30_cppui255},
{0xd88ddfeed400a8755596b21942c1497e_cppui255, 0x114c302e6118290f91e6772976041fa1_cppui255},
{0x87eb0ddba57e35f6d286673802a4af59_cppui255, 0x75e22506c7cf4c64bb6be5ee11527f2c_cppui255},
{0x26846476fd5fc54a5d43385167c95144_cppui255, 0xf2643f533cc85bb9d16b782f8d7db193_cppui255},
{0x506d86582d252405b840018792cad2bf_cppui255, 0x1259f1ef5aa5f887e13cb2f0094f51e1_cppui255},
{0xffff0ad7e659772f9534c195c815efc4_cppui255, 0x14ef1e1daed4404c06385d11192e92b_cppui255},
{0x6cf04127db05441cd833107a52be8528_cppui255, 0x68890e4317e6a02ab47683aa75964220_cppui255},
{0xb7d05f875f140027ef5118a2247bbb84_cppui255, 0xce8f2f0f1123623085daf7960c329f5f_cppui255},
{0xdf6af5f5bbdb6be9ef8aa618e4bf8073_cppui255, 0x960867171e29676f8b284dea6a08a85e_cppui255},
{0xb58d900f5e182e3c50ef74969ea16c77_cppui255, 0x26c549757cc23523c369587da7293784_cppui255},
{0xd49a7502ffcfb0340b1d7885688500ca_cppui255, 0x308161a7f96b62df9d083b71fcc8f2bb_cppui255},
{0x8fe6b1689256c0d385f42f5bbe2027a2_cppui255, 0x2c1996e110ba97c171d3e5948de92beb_cppui255},
{0x8d0d63c39ebade8509e0ae3c9c3876fb_cppui255, 0x5fa112be18f905ecacfecb92057603ab_cppui255},
{0x95eec8b2e541cad4e91de38385f2e046_cppui255, 0x619f54496c2382cb6cacd5b98c26f5a4_cppui255},
{0xf893e908917775b62bff23294dbbe3a1_cppui255, 0xcd8e6cc1c35b4801887b646a6f81f17f_cppui255},
{0xcddba7b592e3133393c16194fac7431a_cppui255, 0xbf2f5485ed711db282183c819e08ebaa_cppui255},
{0x8a8d7fe3af8caa085a7639a832001457_cppui255, 0xdfb9128a8061142ad0335629ff23ff9c_cppui255},
{0xfeb3c337d7a51a6fbf00b9e34c52e1c9_cppui255, 0x195c969bd4e7a0bfd51d5c5bed9c1167_cppui255},
{0xe71f0aa83cc32edfbefa9f4d3e0174ca_cppui255, 0x85182eec9f3a09f6a6c0df6377a510d7_cppui255},
{0x31206fa80a50bb6abe29085058f16212_cppui255, 0x212a60eec8f049fecb92d8c8e0a84bc0_cppui255},
{0x21352bfecbeddde993839f614c3dac0a_cppui255, 0x3ee37543f9b412b16199dc158e23b544_cppui255},
{0x619e312724bb6d7c3153ed9de791d764_cppui255, 0xa366b389af13c58bf8a8d90481a46765_cppui255},
{0x7cdd2986268250628d0c10e385c58c61_cppui255, 0x91e6fbe05191bcc04f133f2cea72c1c4_cppui255},
{0x848930bd7ba8cac54661072113fb2788_cppui255, 0x69e07bb8587f91392933374d017bcbe1_cppui255},
{0x8869ff2c22b28cc10510d98532928033_cppui255, 0x28be4fb0e80495e8bb8d271f5b889636_cppui255},
{0xb5fe28e79f1b850f8658246ce9b6a1e7_cppui255, 0xb49fc06db7143e8fe0b4f2b0c5523a5c_cppui255},
{0x985e929f70af28d0bdd1a90a808f977f_cppui255, 0x597c7c778c489e98d3bd8910d31ac0f7_cppui255},
{0xc6f67e02e6e4e1bdefb994c6098953f3_cppui255, 0x4636ba2b6ca20a4721d2b26a886722ff_cppui255},
{0x1c9a7e5ff1cf48b4ad1582d3f4e4a100_cppui255, 0x4f3b20d8c5a2b71387a4254ad933ebc5_cppui255},
{0x2f075ae229646b6f6aed19a5e372cf29_cppui255, 0x5081401eb893ff599b3f9acc0c0d3e7d_cppui255},
{0x328921deb59612076801e8cd61592107_cppui255, 0xb5c67c79b846595cc6320c395b46362c_cppui255},
{0xbfb909fdb236ad2411b4e4883810a074_cppui255, 0xb840464689986c3f8a8091827e17c327_cppui255},
{0x55d8fb3687ba3ba49f342c77f5a1f89b_cppui255, 0xec83d811446e1a467139213d640b6a74_cppui255},
{0xf7210d4f8e7e1039790e7bf4efa20755_cppui255, 0x5a10a6db1dd4b95da313aaa88b88fe76_cppui255},
{0xad21b516cbc645ffe34ab5de1c8aef8c_cppui255, 0xd4e7f8d2b51e8e1456adc7563cda206f_cppui255}
}};
// The more precomputed powers of two are uncommented, the more RAM is used during circuit generation phase.
constexpr std::array<unsigned long long, 11> precomputed_powers_of_two = {{
1,
2,
4,
8,
16,
32,
64,
128,
256,
512,
1024,
// 2048,
// 4096,
// 8192,
// 16384,
// 32768,
// 65536,
// 131072,
// 262144,
// 524288,
// 1048576,
// 2097152,
// 4194304,
// 8388608,
// 16777216,
// 33554432,
// 67108864,
// 134217728,
// 268435456,
// 536870912,
// 1073741824,
// 2147483648,
// 4294967296,
// 8589934592,
// 17179869184,
// 34359738368,
// 68719476736,
// 137438953472,
// 274877906944
}};

bool is_same(typename hashes::sha2<256>::block_type block0,
    typename hashes::sha2<256>::block_type block1){

    return block0[0] == block1[0] && block0[1] == block1[1];
}

template <std::size_t LayerSize>
typename hashes::sha2<256>::block_type hash_layer(std::array<typename hashes::sha2<256>::block_type, LayerSize> input){
    std::array<typename hashes::sha2<256>::block_type, LayerSize / 2> next_layer;

    for (std::size_t leaf_index = 0; leaf_index < LayerSize / 2; leaf_index++) {
        next_layer[leaf_index] =
            hash<hashes::sha2<256>>(input[2 * leaf_index], input[2 * leaf_index + 1]);
    }
    if (LayerSize == 2)
        return next_layer[0];
    else
        return hash_layer<LayerSize / 2>(next_layer);
}

[[circuit]] bool balance_tree(
    [[private]] std::array<int64_t, precomputed_powers_of_two[validators_amount_log2]> validator_balances,
    typename hashes::sha2<256>::block_type expected_root,
    unsigned long long expected_total_balance) {

    constexpr std::size_t validators_amount = precomputed_powers_of_two[validators_amount_log2];
    constexpr std::size_t potentially_non_zero_leaves_amount_log2 = validators_amount_log2 - validators_per_leaf_log2;
    constexpr std::size_t potentially_non_zero_leaves_amount = precomputed_powers_of_two[potentially_non_zero_leaves_amount_log2];

    std::array<typename hashes::sha2<256>::block_type, potentially_non_zero_leaves_amount> potentially_non_zero_leaves;
    for (std::size_t i = 0; i < potentially_non_zero_leaves_amount; i++) {
        // MSB first
        typedef __zkllvm_field_pallas_base __attribute__((ext_vector_type(64))) decomposed_int64_type;
        typedef __zkllvm_field_pallas_base __attribute__((ext_vector_type(128))) decomposed_int128_type;

        decomposed_int64_type first_balance_in_block_bits =
            __builtin_assigner_bit_decomposition64(validator_balances[4*i]);
        decomposed_int64_type second_balance_in_block_bits =
            __builtin_assigner_bit_decomposition64(validator_balances[4*i+1]);
        decomposed_int128_type first_composition_input;
        for (std::size_t j = 0; j < 64; j++) {
            first_composition_input[j] =       first_balance_in_block_bits[j];
            first_composition_input[j + 64] = second_balance_in_block_bits[j];
        }

        decomposed_int64_type third_balance_in_block_bits =
            __builtin_assigner_bit_decomposition64(validator_balances[4*i+2]);
        decomposed_int64_type fourth_balance_in_block_bits =
            __builtin_assigner_bit_decomposition64(validator_balances[4*i+3]);
        decomposed_int128_type second_composition_input;
        for (std::size_t j = 0; j < 64; j++) {
            second_composition_input[j] =       third_balance_in_block_bits[j];
            second_composition_input[j + 64] = fourth_balance_in_block_bits[j];
        }

        typename algebra::curves::pallas::base_field_type::value_type first_block =
            __builtin_assigner_bit_composition128(first_composition_input);
        typename algebra::curves::pallas::base_field_type::value_type second_block =
            __builtin_assigner_bit_composition128(second_composition_input);

        potentially_non_zero_leaves[i] = {first_block, second_block};
    }

    unsigned long long total_balance = 0;
    for (std::size_t i = 0; i < validators_amount; i++) {
        total_balance += validator_balances[i];
    }

    typename hashes::sha2<256>::block_type non_zero_balances_subtree_root = hash_layer<potentially_non_zero_leaves_amount>(potentially_non_zero_leaves);

    constexpr std::size_t leaves_total_amount_log2 = precomputed_powers_of_two.size() - 1;
    constexpr std::size_t layer_with_zero_balance_subtrees_roots_size_log2 = leaves_total_amount_log2 - potentially_non_zero_leaves_amount_log2;
    constexpr std::size_t layer_with_zero_balance_subtrees_roots_size = precomputed_powers_of_two[layer_with_zero_balance_subtrees_roots_size_log2];

    std::array<typename hashes::sha2<256>::block_type, layer_with_zero_balance_subtrees_roots_size> layer_with_zero_balance_subtrees_roots;
    layer_with_zero_balance_subtrees_roots[0] = non_zero_balances_subtree_root;
    for (std::size_t i = 1; i < layer_with_zero_balance_subtrees_roots_size; i++) {
        layer_with_zero_balance_subtrees_roots[i] = {
            precomputed_zero_hashes[potentially_non_zero_leaves_amount_log2][0],
            precomputed_zero_hashes[potentially_non_zero_leaves_amount_log2][1]
        };
    }

    typename hashes::sha2<256>::block_type root = hash_layer<layer_with_zero_balance_subtrees_roots_size>(layer_with_zero_balance_subtrees_roots);

    return (is_same(expected_root, root) && (expected_total_balance == total_balance));
}