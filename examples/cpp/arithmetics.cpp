#include <nil/crypto3/algebra/curves/pallas.hpp>
#include <cstdint>

static_assert(SIZE_MAX > 0, "");
using namespace nil::crypto3::algebra::curves;
typename pallas::base_field_type::value_type pow_2(typename pallas::base_field_type::value_type a) {
    typename pallas::base_field_type::value_type res = 1;
    for (int i = 0; i < 2; ++i) {
        res *= a;
    }
    return res;
}

[[circuit]] typename pallas::base_field_type::value_type
    field_arithmetic_example(typename pallas::base_field_type::value_type a,
                             typename pallas::base_field_type::value_type b) {

    typename pallas::base_field_type::value_type c = (a + b) * a + b * (a + b) * (a + b);
    const typename pallas::base_field_type::value_type constant = 0x12345678901234567890_cppui255;
    return c * c * c / (b - a) + pow_2(a) + constant;
}
