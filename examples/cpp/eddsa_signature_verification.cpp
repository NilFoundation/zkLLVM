#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

typedef __attribute__((ext_vector_type(4)))
                typename pallas::base_field_type::value_type eddsa_message_block_type;

[[circuit]] bool verify_eddsa_signature (
    
    __zkllvm_curve_curve25519 input_R, 
    __zkllvm_field_curve25519_scalar input_s, 
    __zkllvm_curve_curve25519 pk,
    eddsa_message_block_type M,
    __zkllvm_curve_curve25519 B) { // TODO replace with constructor

        __zkllvm_field_curve25519_scalar k = __builtin_assigner_sha2_512_curve25519(input_R, pk, M);

        if ((B*input_s - (input_R + (pk*k)) ) == B) { // TODO replace with .is_zero()
            return true;
        }
        else{
            return false;
        }
    }