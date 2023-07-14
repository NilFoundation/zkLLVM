#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>

using namespace nil::crypto3;
using namespace nil::crypto3::algebra::curves;

typedef __attribute__((ext_vector_type(4)))
                typename pallas::base_field_type::value_type eddsa_message_block_type;


[[circuit]] __zkllvm_field_curve25519_scalar verify_eddsa_signature (
    __zkllvm_curve_curve25519 R, 
    __zkllvm_curve_curve25519 pk, 
    eddsa_message_block_type M
    ) {

        __zkllvm_field_curve25519_scalar k = __builtin_assigner_sha2_512_curve25519(R, pk, M);

        return k;
}