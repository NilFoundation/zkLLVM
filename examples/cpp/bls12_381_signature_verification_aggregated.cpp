#include <nil/crypto3/algebra/fields/bls12/base_field.hpp>
#include <nil/crypto3/algebra/curves/bls12.hpp>
#include <nil/crypto3/algebra/algorithms/pair.hpp>

using namespace nil::crypto3;

constexpr const std::size_t validators_amount = 5;

[[circuit]] bool verify_aggregated_signature(
    typename algebra::fields::bls12_base_field<381>::value_type hashed_msg,
    std::array<typename algebra::curves::bls12<381>::template g2_type<>::value_type, validators_amount>
        pubkeys,
    typename algebra::curves::bls12<381>::template g1_type<>::value_type aggregated_signature) {

    // __builtin_assigner_exit_check(__builtin_assigner_is_in_g1_check(aggregated_signature));

    typename algebra::curves::bls12<381>::template g2_type<>::value_type g2_group_generator =
        algebra::curves::bls12<381>::template g2_type<>::one();
    typename algebra::curves::bls12<381>::gt_type::value_type pairing1 =
        algebra::pair<algebra::curves::bls12<381>>(aggregated_signature, g2_group_generator);

    typename algebra::curves::bls12<381>::template g1_type<>::value_type msg_point =
        __builtin_assigner_hash_to_curve(hashed_msg);

    // __builtin_assigner_exit_check(__builtin_assigner_is_in_g2_check(pubkeys[0]));
    typename algebra::curves::bls12<381>::gt_type::value_type pairing2 =
        algebra::pair<algebra::curves::bls12<381>>(msg_point, pubkeys[0]);

    typename algebra::curves::bls12<381>::gt_type::value_type current_pairing;

    for (std::size_t i = 1; i < validators_amount; i++) {
        // __builtin_assigner_exit_check(__builtin_assigner_is_in_g2_check(pubkeys[i]));
        current_pairing = algebra::pair<algebra::curves::bls12<381>>(msg_point, pubkeys[i]);
        pairing2 = __builtin_assigner_gt_multiplication(pairing2, current_pairing);
    }

    bool are_equal = 0;
    for (std::size_t i = 0; i < 12; i++) {
        are_equal = are_equal && (pairing1[i] == pairing2[i]);
    }
    // __builtin_assigner_exit_check(are_equal);

    return are_equal;
}
