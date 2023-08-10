#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

typedef __attribute__((ext_vector_type(64)))
                typename pallas::base_field_type::value_type decomposed_type;


[[circuit]] decomposed_type remainder(uint64_t input) {

    return __builtin_assigner_bit_decomposition64(input);
}
