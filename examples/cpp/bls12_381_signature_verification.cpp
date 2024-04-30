#include <nil/crypto3/algebra/fields/bls12/base_field.hpp>
#include <nil/crypto3/algebra/curves/bls12.hpp>
#include <nil/crypto3/algebra/algorithms/pair.hpp>

using namespace nil::crypto3;

[[circuit]] bool verify_signature(typename algebra::fields::bls12_base_field<381>::value_type hashed_msg,
                                  typename algebra::curves::bls12<381>::template g2_type<>::value_type pubkey,
                                  typename algebra::curves::bls12<381>::template g1_type<>::value_type sig) {

    typename algebra::curves::bls12<381>::template g1_type<>::value_type msg_point =
        __builtin_assigner_hash_to_curve(hashed_msg);

    // __builtin_assigner_exit_check(__builtin_assigner_is_in_g1_check(sig));
    // __builtin_assigner_exit_check(__builtin_assigner_is_in_g2_check(pubkey));

    typename algebra::curves::bls12<381>::template g2_type<>::value_type g2_group_generator =
        algebra::curves::bls12<381>::template g2_type<>::one();

    typename algebra::curves::bls12<381>::gt_type::value_type pairing1 =
        algebra::pair<algebra::curves::bls12<381>>(sig, g2_group_generator);
    typename algebra::curves::bls12<381>::gt_type::value_type pairing2 =
        algebra::pair<algebra::curves::bls12<381>>(msg_point, pubkey);

    bool are_equal = 0;
    for (std::size_t i = 0; i < 12; i++) {
        are_equal = are_equal && (pairing1[i] == pairing2[i]);
    }
    // __builtin_assigner_exit_check(are_equal);

    return are_equal;
}
