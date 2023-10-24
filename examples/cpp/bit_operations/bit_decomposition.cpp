#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

constexpr bool is_msb = true;
constexpr std::size_t bits_amount = 64;

[[circuit]] std::array<typename pallas::base_field_type::value_type, bits_amount>
        decompose(uint64_t input) {

    std::array<typename pallas::base_field_type::value_type, bits_amount> result;

    __builtin_assigner_bit_decomposition(result.data(), bits_amount, input, is_msb);

    return result;
}
