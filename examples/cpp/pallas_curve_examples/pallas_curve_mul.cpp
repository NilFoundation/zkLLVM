#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] typename pallas::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type
    curve_mul(typename pallas::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type point,
              typename pallas::scalar_field_type::value_type scalar) {

    return point * scalar;
}
