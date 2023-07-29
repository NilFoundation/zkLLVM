#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>
#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3;
using namespace nil::crypto3::algebra::curves;

typedef __attribute__((ext_vector_type(4)))
                typename pallas::base_field_type::value_type eddsa_message_block_type;

[[circuit]] bool verify_protocol_state_proof (
    typename hashes::sha2<256>::block_type last_confirmed_block_hash,
    std::array< typename hashes::sha2<256>::block_type,            64> unconfirmed_blocks_prev_block_hash,
    std::array< typename hashes::sha2<256>::block_type,            64> unconfirmed_blocks_data,
    std::array< __zkllvm_curve_curve25519,                         64> unconfirmed_blocks_validators_signature_R,
    std::array< __zkllvm_field_curve25519_scalar,                  64> unconfirmed_blocks_validators_signature_s,
    std::array< __zkllvm_curve_curve25519,                         64> unconfirmed_blocks_validators_key) 
    {
        return true;
}