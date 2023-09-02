#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

typedef __attribute__((ext_vector_type(64)))
                typename pallas::base_field_type::value_type decomposed_type;


[[circuit]] decomposed_type decompose(uint64_t input) {

    bool is_msb = true;
    return __builtin_assigner_bit_decomposition64(input, is_msb);
}
