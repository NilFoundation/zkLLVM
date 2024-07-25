#include <crypto3-algebra/include/nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

constexpr bool is_msb = false;

[[circuit]] typename pallas::base_field_type::value_type compose(
    std::array<typename pallas::base_field_type::value_type, 128> input) {

    return __builtin_assigner_bit_composition(input.data(), 128, is_msb);
}
