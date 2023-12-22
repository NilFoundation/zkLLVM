#include <nil/crypto3/algebra/fields/bls12/base_field.hpp>
#include <nil/crypto3/algebra/curves/bls12.hpp>
#include <nil/crypto3/pubkey/detail/bls/bls_basic_functions.hpp>


using namespace nil::crypto3;

constexpr const std::size_t validators_amount = 5;

[[circuit]] bool verify_aggregated_signature(
    typename algebra::fields::bls12_base_field<381>::value_type hashed_msg,
    std::array<typename algebra::curves::bls12<381>::template g2_type<>::value_type, validators_amount> pubkeys,
    typename algebra::curves::bls12<381>::template g1_type<>::value_type aggregated_signature
) {

    return nil::crypto3::pubkey::detail::bls_basic_functions::aggregate_verify<validators_amount>(hashed_msg, pubkeys, aggregated_signature);
}
