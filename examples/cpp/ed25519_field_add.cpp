#include <nil/crypto3/algebra/curves/ed25519.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] typename ed25519::base_field_type::value_type field_operations(
        typename ed25519::base_field_type::value_type a,
        typename ed25519::base_field_type::value_type b) {
    return a + b;
}