#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

typedef __attribute__((ext_vector_type(128)))
                typename pallas::base_field_type::value_type composed_type;


[[circuit]] pallas::base_field_type::value_type remainder(composed_type input) {

    return __builtin_assigner_bit_composition128(input);
}
