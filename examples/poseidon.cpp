#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/poseidon.hpp>

using namespace nil::crypto3;
using namespace nil::crypto3::algebra::curves;

[[circuit]] typename pallas::base_field_type::value_type poseidon_example1(typename pallas::base_field_type::value_type b,
                                                                   typename pallas::base_field_type::value_type c) {

    typename pallas::base_field_type::value_type hash_result = hash<hashes::poseidon>(b, c);

    return hash_result;
}
