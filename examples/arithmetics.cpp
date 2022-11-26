#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;
typename pallas::base_field_type::value_type square(
    typename pallas::base_field_type::value_type a) {
    return a * a;
}

[[circuit]] typename pallas::base_field_type::value_type field_arithmetic_example(
	typename pallas::base_field_type::value_type a,
	typename pallas::base_field_type::value_type b) {

    typename pallas::base_field_type::value_type c = (a + b)*a + b*(a+b)*(a+b);
    return c*c*c/(b - a) + square(a);
}
