#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

typedef __attribute__((ext_vector_type(2))) typename pallas::base_field_type::value_type thetas_type;

typedef __attribute__((ext_vector_type(3))) typename pallas::base_field_type::value_type output_type;

constexpr std::size_t size = 7;

[[circuit]] output_type permargver(std::array<typename pallas::base_field_type::value_type, size> f,
                                   std::array<typename pallas::base_field_type::value_type, size>
                                       se,
                                   std::array<typename pallas::base_field_type::value_type, size>
                                       sigma,
                                   typename pallas::base_field_type::value_type L0,
                                   typename pallas::base_field_type::value_type V,
                                   typename pallas::base_field_type::value_type V_zeta,
                                   typename pallas::base_field_type::value_type q_last,
                                   typename pallas::base_field_type::value_type q_pad,
                                   thetas_type thetas) {

    return __builtin_assigner_permutation_arg_verifier(
        f.data(), se.data(), sigma.data(), size, L0, V, V_zeta, q_last, q_pad, thetas);
}
