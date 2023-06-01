#include <nil/crypto3/algebra/fields/curve25519/base_field.hpp>
#include <nil/crypto3/algebra/fields/curve25519/scalar_field.hpp>
#include <nil/crypto3/algebra/curves/ed25519.hpp>

using namespace nil::crypto3;
using namespace nil::crypto3::algebra::curves;

[[circuit]] typename ed25519::template g1_type<>::value_type curve_addition (
    typename ed25519::template g1_type<>::value_type x, 
    typename ed25519::template g1_type<>::value_type y) {
    return x + y;
}