#include <nil/crypto3/algebra/curves/pallas.hpp>

#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>

using namespace nil::crypto3::algebra::curves;
using namespace nil::crypto3;
using namespace nil::marshalling;

[[circuit]] typename hashes::sha2<256>::block_type
    balance(std::array<typename algebra::pallas::value_type, 0x100000> layer_0_leaves) {

    std::array<typename algebra::pallas::value_type, 0x80000> layer_1_leaves;
    std::array<typename algebra::pallas::value_type, 0x40000> layer_2_leaves;
    std::array<typename algebra::pallas::value_type, 0x20000> layer_3_leaves;
    std::array<typename algebra::pallas::value_type, 0x10000> layer_4_leaves;
    std::array<typename algebra::pallas::value_type, 0x8000> layer_5_leaves;
    std::array<typename algebra::pallas::value_type, 0x4000> layer_6_leaves;
    std::array<typename algebra::pallas::value_type, 0x2000> layer_7_leaves;
    std::array<typename algebra::pallas::value_type, 0x1000> layer_8_leaves;
    std::array<typename algebra::pallas::value_type, 0x800> layer_9_leaves;
    std::array<typename algebra::pallas::value_type, 0x400> layer_10_leaves;
    std::array<typename algebra::pallas::value_type, 0x200> layer_11_leaves;
    std::array<typename algebra::pallas::value_type, 0x100> layer_12_leaves;
    std::array<typename algebra::pallas::value_type, 0x80> layer_13_leaves;
    std::array<typename algebra::pallas::value_type, 0x40> layer_14_leaves;
    std::array<typename algebra::pallas::value_type, 0x20> layer_15_leaves;
    std::array<typename algebra::pallas::value_type, 0x10> layer_16_leaves;
    std::array<typename algebra::pallas::value_type, 0x8> layer_17_leaves;
    std::array<typename algebra::pallas::value_type, 0x4> layer_18_leaves;
    std::array<typename algebra::pallas::value_type, 0x2> layer_19_leaves;
    typename algebra::pallas::value_type root;

    for (std::size_t LeafIndex = 0; LeafIndex < layer_1_leaves.size(); LeafIndex++) {
        layer_1_leaves[LeafIndex] =
            hash<hashes::sha2<256>>(layer_0_leaves[2 * LeafIndex], layer_0_leaves[2 * LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_2_leaves.size(); LeafIndex++) {
        layer_2_leaves[LeafIndex] =
            hash<hashes::sha2<256>>(layer_1_leaves[2 * LeafIndex], layer_1_leaves[2 * LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_3_leaves.size(); LeafIndex++) {
        layer_3_leaves[LeafIndex] =
            hash<hashes::sha2<256>>(layer_2_leaves[2 * LeafIndex], layer_2_leaves[2 * LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_4_leaves.size(); LeafIndex++) {
        layer_4_leaves[LeafIndex] =
            hash<hashes::sha2<256>>(layer_3_leaves[2 * LeafIndex], layer_3_leaves[2 * LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_5_leaves.size(); LeafIndex++) {
        layer_5_leaves[LeafIndex] =
            hash<hashes::sha2<256>>(layer_4_leaves[2 * LeafIndex], layer_4_leaves[2 * LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_6_leaves.size(); LeafIndex++) {
        layer_6_leaves[LeafIndex] =
            hash<hashes::sha2<256>>(layer_5_leaves[2 * LeafIndex], layer_5_leaves[2 * LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_7_leaves.size(); LeafIndex++) {
        layer_7_leaves[LeafIndex] =
            hash<hashes::sha2<256>>(layer_6_leaves[2 * LeafIndex], layer_6_leaves[2 * LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_8_leaves.size(); LeafIndex++) {
        layer_8_leaves[LeafIndex] =
            hash<hashes::sha2<256>>(layer_7_leaves[2 * LeafIndex], layer_7_leaves[2 * LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_9_leaves.size(); LeafIndex++) {
        layer_9_leaves[LeafIndex] =
            hash<hashes::sha2<256>>(layer_8_leaves[2 * LeafIndex], layer_8_leaves[2 * LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_10_leaves.size(); LeafIndex++) {
        layer_10_leaves[LeafIndex] =
            hash<hashes::sha2<256>>(layer_9_leaves[2 * LeafIndex], layer_9_leaves[2 * LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_11_leaves.size(); LeafIndex++) {
        layer_11_leaves[LeafIndex] =
            hash<hashes::sha2<256>>(layer_10_leaves[2 * LeafIndex], layer_10_leaves[2 * LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_12_leaves.size(); LeafIndex++) {
        layer_12_leaves[LeafIndex] =
            hash<hashes::sha2<256>>(layer_11_leaves[2 * LeafIndex], layer_11_leaves[2 * LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_13_leaves.size(); LeafIndex++) {
        layer_13_leaves[LeafIndex] =
            hash<hashes::sha2<256>>(layer_12_leaves[2 * LeafIndex], layer_12_leaves[2 * LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_14_leaves.size(); LeafIndex++) {
        layer_14_leaves[LeafIndex] =
            hash<hashes::sha2<256>>(layer_13_leaves[2 * LeafIndex], layer_13_leaves[2 * LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_15_leaves.size(); LeafIndex++) {
        layer_15_leaves[LeafIndex] =
            hash<hashes::sha2<256>>(layer_14_leaves[2 * LeafIndex], layer_14_leaves[2 * LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_16_leaves.size(); LeafIndex++) {
        layer_16_leaves[LeafIndex] =
            hash<hashes::sha2<256>>(layer_15_leaves[2 * LeafIndex], layer_15_leaves[2 * LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_17_leaves.size(); LeafIndex++) {
        layer_17_leaves[LeafIndex] =
            hash<hashes::sha2<256>>(layer_16_leaves[2 * LeafIndex], layer_16_leaves[2 * LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_18_leaves.size(); LeafIndex++) {
        layer_18_leaves[LeafIndex] =
            hash<hashes::sha2<256>>(layer_17_leaves[2 * LeafIndex], layer_17_leaves[2 * LeafIndex + 1]);
    }

    for (std::size_t LeafIndex = 0; LeafIndex < layer_19_leaves.size(); LeafIndex++) {
        layer_19_leaves[LeafIndex] =
            hash<hashes::sha2<256>>(layer_18_leaves[2 * LeafIndex], layer_18_leaves[2 * LeafIndex + 1]);
    }

    root = hash<hashes::sha2<256>>(layer_19_leaves[0], layer_19_leaves[1]);

    return root;
}