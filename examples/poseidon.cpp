#include <nil/crypto3/algebra/curves/pallas.hpp>

namespace nil{
    namespace crypto3{
        namespace hashes {
            typedef __attribute__((ext_vector_type(3))) typename algebra::curves::pallas::base_field_type::value_type poseidon_block_type;

            poseidon_block_type poseidon(poseidon_block_type input_block);
        }
    }
}

using namespace nil::crypto3;
using namespace nil::crypto3::algebra::curves;

[[circuit]] typename hashes::poseidon_block_type poseidon_example1(
    typename pallas::base_field_type::value_type a,
    typename pallas::base_field_type::value_type b,
    typename pallas::base_field_type::value_type c) {

    typename hashes::poseidon_block_type input_block = {a + b, b+c, a*c};

    typename hashes::poseidon_block_type hash_result = hashes::poseidon(input_block);

    return hash_result;
}
