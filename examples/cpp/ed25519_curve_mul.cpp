#include <nil/crypto3/algebra/curves/ed25519.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] typename ed25519::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type
    curve_mul(typename ed25519::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type point,
              typename ed25519::scalar_field_type::value_type scalar) {

    return point * scalar;
}
