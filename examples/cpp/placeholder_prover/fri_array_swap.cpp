#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

constexpr const std::size_t size = 6;

[[circuit]] std::array<typename pallas::base_field_type::value_type, size>
    field_arithmetic_example(typename pallas::base_field_type::value_type swap_field,
        std::array<typename pallas::base_field_type::value_type, size> input) {

    bool swap = typename pallas::base_field_type::value_type(0) != swap_field;

    std::array<typename pallas::base_field_type::value_type, size> res;
    for (std::size_t i = 0; i < size; i+=2) {
        if (swap) {
            res[i] = input[i+1];
            res[i+1] = input[i];
        }
        else {
            res[i] = input[i];
            res[i+1] = input[i+1];
        }
    }
    return res;
}
