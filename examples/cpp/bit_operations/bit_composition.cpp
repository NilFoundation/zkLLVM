#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

typedef __attribute__((ext_vector_type(64)))
                typename pallas::base_field_type::value_type composed_type;


[[circuit]] pallas::base_field_type::value_type compose(composed_type input1, composed_type input2) {

    bool is_msb = false;
    return __builtin_assigner_bit_composition128(input1, input2, is_msb);
}
