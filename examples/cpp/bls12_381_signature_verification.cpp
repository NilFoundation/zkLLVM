#include <nil/crypto3/algebra/fields/bls12/base_field.hpp>
#include <nil/crypto3/algebra/curves/bls12.hpp>
#include <nil/crypto3/pubkey/detail/bls/bls_basic_functions.hpp>

using namespace nil::crypto3;

[[circuit]] bool verify_signature(
    typename algebra::fields::bls12_base_field<381>::value_type hashed_msg,
    typename algebra::curves::bls12<381>::template g2_type<>::value_type pubkey,
    typename algebra::curves::bls12<381>::template g1_type<>::value_type sig
) {
    return pubkey::detail::bls_basic_functions::verify(hashed_msg, pubkey, sig);
}