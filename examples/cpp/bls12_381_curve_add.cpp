#include <nil/crypto3/algebra/curves/bls12.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] typename bls12<381>::template g1_type<>::value_type hash_to_curve(
    typename bls12<381>::template g1_type<>::value_type a,
    typename bls12<381>::template g1_type<>::value_type b) {

    return a + b;
}
