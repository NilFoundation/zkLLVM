#include <nil/crypto3/algebra/fields/bls12/base_field.hpp>

using namespace nil::crypto3::algebra::fields;

[[circuit]] typename bls12_base_field<381>::value_type mul(
    typename bls12_base_field<381>::value_type a,
    typename bls12_base_field<381>::value_type b) {

    return a * b;
}
