#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] typename pallas::base_field_type::value_type
    field_arithmetic_example([[private_input]] typename pallas::base_field_type::value_type a,
                             typename pallas::base_field_type::value_type b) {

    return a + b;
}
