#include <nil/crypto3/algebra/curves/bls12.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] typename bls12<381>::base_field_type::value_type add(typename bls12<381>::base_field_type::value_type a,
                                                                 typename bls12<381>::base_field_type::value_type b) {

    return a + b;
}
