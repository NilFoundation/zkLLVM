#include <nil/crypto3/algebra/fields/curve25519/base_field.hpp>
#include <nil/crypto3/algebra/fields/curve25519/scalar_field.hpp>
#include <nil/crypto3/algebra/curves/ed25519.hpp>
#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

typedef __attribute__((ext_vector_type(4)))
                typename pallas::base_field_type::value_type eddsa_message_block_type;

struct eddsa_signature_type {
    typename ed25519::template g1_type<>::value_type R;
    typename ed25519::scalar_field_type::value_type s;
};

[[circuit]] bool verify_eddsa_signature (eddsa_signature_type input, 
    typename ed25519::template g1_type<>::value_type pk,
    eddsa_message_block_type M) {

        typename ed25519::template g1_type<>::value_type B = ed25519::template g1_type<>::value_type::one();
        __zkllvm_field_curve25519_scalar k = __builtin_assigner_sha2_512_curve25519(input.R.data, pk.data, M);

        return (B*input.s - (input.R + (pk*k))).is_zero();
    }