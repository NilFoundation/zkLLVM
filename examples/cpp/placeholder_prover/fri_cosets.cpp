#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

constexpr std::size_t bits_amount = 64;
constexpr std::size_t array_length = 3;
constexpr typename pallas::base_field_type::value_type omega =
    0x1ff2863fd35bfc59e51f3693bf37e2d841d1b5fbed4138f755a638bec8750abd_cppui255;

[[circuit]] std::array<std::array<typename pallas::base_field_type::value_type, 3>, array_length>
    fri_cosets(typename pallas::base_field_type::value_type input) {

    std::array<std::array<typename pallas::base_field_type::value_type, 3>, array_length> res;

    __builtin_assigner_fri_cosets(res.data(), array_length, omega, input);
    return res;
}
