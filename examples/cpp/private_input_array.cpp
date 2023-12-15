#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] std::array<typename pallas::base_field_type::value_type, 2>
    field_arithmetic_example([[private_input]] std::array<typename pallas::base_field_type::value_type, 2> a,
                             std::array<typename pallas::base_field_type::value_type, 2> b) {

    std::array<typename pallas::base_field_type::value_type, 2> res;
    res[0] = a[0] + b[0];
    res[1] = a[1] + b[1];

    return res;
}
