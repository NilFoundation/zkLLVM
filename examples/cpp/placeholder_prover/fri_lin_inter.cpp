#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] typename pallas::base_field_type::value_type
    field_arithmetic_example(typename pallas::base_field_type::value_type s,
                             typename pallas::base_field_type::value_type y0,
                             typename pallas::base_field_type::value_type y1,
                             typename pallas::base_field_type::value_type alpha) {

    return __builtin_assigner_fri_lin_inter(s, y0, y1, alpha);
}
