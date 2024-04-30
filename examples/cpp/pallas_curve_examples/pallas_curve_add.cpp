#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] typename pallas::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type
    curve_operations(
        typename pallas::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type a,
        typename pallas::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type b) {

    return a + b;
}
