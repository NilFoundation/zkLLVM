
#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/poseidon.hpp>

using namespace nil::crypto3;
using namespace nil::crypto3::algebra::curves;


typename pallas::base_field_type::value_type evaluate_root (
    typename pallas::base_field_type::value_type a,
    typename pallas::base_field_type::value_type b,
    typename pallas::base_field_type::value_type c,
    typename pallas::base_field_type::value_type d
) {
    typename pallas::base_field_type::value_type ab =  hash<hashes::poseidon>(a, b);
    typename pallas::base_field_type::value_type cd =  hash<hashes::poseidon>(c, d);
    return hash<hashes::poseidon>(ab, cd);
}


[[circuit]] bool merkle_tree_poseidon (
    typename pallas::base_field_type::value_type expected_root,
    [[private_input]] std::array<typename pallas::base_field_type::value_type, 64> inp
) {

    std::array<typename pallas::base_field_type::value_type, 16> layer_1;
    std::array<typename pallas::base_field_type::value_type, 4> layer_2;
    typename pallas::base_field_type::value_type real_root;

    layer_1[0] =  evaluate_root(inp[0],  inp[1],  inp[2],  inp[3]);;
    #pragma zk_multi_prover 1
      {layer_1[1] =  evaluate_root(inp[4],  inp[5],  inp[6], inp[7]);}
    #pragma zk_multi_prover 2
      {layer_1[2] =  evaluate_root(inp[8],  inp[9],  inp[10], inp[11]);}
    #pragma zk_multi_prover 3
      {layer_1[3] =  evaluate_root(inp[12], inp[13], inp[14], inp[15]);}
    #pragma zk_multi_prover 4
      {layer_1[4] =  evaluate_root(inp[16], inp[17], inp[18], inp[19]);}
    #pragma zk_multi_prover 5
      {layer_1[5] =  evaluate_root(inp[20], inp[21], inp[22], inp[23]);}
    #pragma zk_multi_prover 6
      {layer_1[6] =  evaluate_root(inp[24], inp[25], inp[26], inp[27]);}
    #pragma zk_multi_prover 7
      {layer_1[7] =  evaluate_root(inp[28], inp[29], inp[30], inp[31]);}
    #pragma zk_multi_prover 8
      {layer_1[8] =  evaluate_root(inp[32], inp[33], inp[34], inp[35]);}
    #pragma zk_multi_prover 9
      {layer_1[9] =  evaluate_root(inp[36], inp[37], inp[38], inp[39]);}
    #pragma zk_multi_prover 10
     {layer_1[10] = evaluate_root(inp[40], inp[41], inp[42], inp[43]);}
    #pragma zk_multi_prover 11
     {layer_1[11] = evaluate_root(inp[44], inp[45], inp[46], inp[47]);}
    #pragma zk_multi_prover 12
     {layer_1[12] = evaluate_root(inp[48], inp[49], inp[50], inp[51]);}
    #pragma zk_multi_prover 13
     {layer_1[13] = evaluate_root(inp[52], inp[53], inp[54], inp[55]);}
    #pragma zk_multi_prover 14
     {layer_1[14] = evaluate_root(inp[56], inp[57], inp[58], inp[59]);}
    #pragma zk_multi_prover 15
     {layer_1[15] = evaluate_root(inp[60], inp[61], inp[62], inp[63]);}

    #pragma zk_multi_prover 16
      {layer_2[0] =  evaluate_root(layer_1[0],  layer_1[1],  layer_1[2],  layer_1[3]);}
    #pragma zk_multi_prover 17
      {layer_2[1] =  evaluate_root(layer_1[4],  layer_1[5],  layer_1[6],  layer_1[7]);}
    #pragma zk_multi_prover 18
      {layer_2[2] =  evaluate_root(layer_1[8],  layer_1[9],  layer_1[10], layer_1[11]);}
    #pragma zk_multi_prover 19
      {layer_2[3] =  evaluate_root(layer_1[12], layer_1[13], layer_1[14], layer_1[15]);}

    #pragma zk_multi_prover 20
      {real_root =  evaluate_root(layer_2[0], layer_2[1], layer_2[2], layer_2[3]);}

    bool output = (real_root == expected_root);
    __builtin_assigner_exit_check(output);
    return output;


}
