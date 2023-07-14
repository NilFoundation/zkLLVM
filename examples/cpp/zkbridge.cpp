#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>
#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3;
using namespace nil::crypto3::algebra::curves;

typedef __attribute__((ext_vector_type(4)))
                typename pallas::base_field_type::value_type eddsa_message_block_type;

bool verify_eddsa_signature (
    __zkllvm_curve_curve25519 input_R, 
    __zkllvm_field_curve25519_scalar input_s, 
    __zkllvm_curve_curve25519 pk,
    eddsa_message_block_type M) {

        // __zkllvm_curve_curve25519 B = 1; // TODO
        __zkllvm_curve_curve25519 B = input_R;
        __zkllvm_field_curve25519_scalar k = __builtin_assigner_sha2_512_curve25519(input_R, pk, M);

        // return (B*input_s - (input_R + (pk*k))).is_zero(); // TODO
        return true;
}

bool is_same(typename hashes::sha2<256>::block_type block0,
    typename hashes::sha2<256>::block_type block1){

    return block0[0] == block1[0] && block0[1] == block1[1];
}

[[circuit]] bool verify_protocol_state_proof (
    typename hashes::sha2<256>::block_type last_confirmed_block_hash,

    std::array< typename hashes::sha2<256>::block_type,            64> unconfirmed_blocks_prev_block_hash,
    std::array< typename hashes::sha2<256>::block_type,            64> unconfirmed_blocks_data,
    std::array< std::array< __zkllvm_curve_curve25519, 16>,        64> unconfirmed_blocks_validators_signatures_R,
    std::array< std::array< __zkllvm_field_curve25519_scalar, 16>, 64> unconfirmed_blocks_validators_signatures_s,
    std::array< std::array< __zkllvm_curve_curve25519, 16>,        64> unconfirmed_blocks_validators_keys,
    std::array< __zkllvm_curve_curve25519,                         64> unconfirmed_blocks_validators_signature_R,
    std::array< __zkllvm_field_curve25519_scalar,                  64> unconfirmed_blocks_validators_signature_s,
    std::array< __zkllvm_curve_curve25519,                         64> unconfirmed_blocks_validators_key) {
    
    for (int i = 0; i < unconfirmed_blocks_prev_block_hash.size(); i++) {
        
        // Check hashes correctness
        if (i == 0) {
            if (!is_same(unconfirmed_blocks_prev_block_hash[i],
                last_confirmed_block_hash)) {
                return false;
            }
        } else {
            typename hashes::sha2<256>::block_type evaluated_block_hash =
                hash<hashes::sha2<256>>(unconfirmed_blocks_prev_block_hash[i-1],
                    unconfirmed_blocks_data[i-1]);
            
            if (!is_same(unconfirmed_blocks_prev_block_hash[i],
                evaluated_block_hash)) {
                return false;
            }
        }

        // Verify signatures
        for (int j = 0; j < 16; j++) {
            eddsa_message_block_type message = {unconfirmed_blocks_prev_block_hash[i][0],
                unconfirmed_blocks_prev_block_hash[i][1],
                unconfirmed_blocks_data[i][0],
                unconfirmed_blocks_data[i][1]};

            if (!verify_eddsa_signature(
                    unconfirmed_blocks_validators_signatures_R[i][j],
                    unconfirmed_blocks_validators_signatures_s[i][j],
                    unconfirmed_blocks_validators_keys[i][j],
                    message)) {
                return false;
            }
        }
    }

    return true;

}