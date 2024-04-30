#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>
#include <nil/crypto3/algebra/curves/pallas.hpp>
#include <nil/crypto3/algebra/curves/ed25519.hpp>

using namespace nil::crypto3;
using namespace nil::crypto3::algebra::curves;

typedef __attribute__((ext_vector_type(4))) typename pallas::base_field_type::value_type eddsa_message_block_type;

typedef struct {
    typename ed25519::template g1_type<>::value_type R;
    typename ed25519::scalar_field_type::value_type s;
} eddsa_signature_type;

bool verify_eddsa_signature(eddsa_signature_type input,
                            typename ed25519::template g1_type<>::value_type pk,
                            eddsa_message_block_type M) {

    typename ed25519::template g1_type<>::value_type B = ed25519::template g1_type<>::one();
    typename ed25519::scalar_field_type::value_type k = __builtin_assigner_sha2_512_curve25519(input.R, pk, M);

    return B * input.s == (input.R + (pk * k));
}

typedef struct {
    typename hashes::sha2<256>::block_type prev_block_hash;
    typename hashes::sha2<256>::block_type data;
    std::array<eddsa_signature_type, 4> validators_signatures;
    std::array<typename ed25519::template g1_type<>::value_type, 4> validators_keys;
} block_data_type;

bool is_same(typename hashes::sha2<256>::block_type block0, typename hashes::sha2<256>::block_type block1) {

    return block0[0] == block1[0] && block0[1] == block1[1];
}

bool verify_signature(block_data_type unconfirmed_block) {
    bool is_verified = true;
    eddsa_message_block_type message = {unconfirmed_block.prev_block_hash[0], unconfirmed_block.prev_block_hash[1],
                                        unconfirmed_block.data[0], unconfirmed_block.data[1]};
    for (int j = 0; j < 4; j++) {
        is_verified = is_verified && verify_eddsa_signature(unconfirmed_block.validators_signatures[j],
                                                            unconfirmed_block.validators_keys[j],
                                                            message);
    }
    return is_verified;
}

[[circuit]] bool verify_protocol_state_proof(typename hashes::sha2<256>::block_type last_confirmed_block_hash,
                                             std::array<block_data_type, 2>
                                                 unconfirmed_blocks) {

    // Check hashes correctness
    bool is_correct = is_same(unconfirmed_blocks[0].prev_block_hash, last_confirmed_block_hash);
    // Verify signatures
    is_correct = is_correct && verify_signature(unconfirmed_blocks[0]);

    for (int i = 1; i < 2; i++) {

        // Check hashes correctness
        typename hashes::sha2<256>::block_type evaluated_block_hash =
            hash<hashes::sha2<256>>(unconfirmed_blocks[i - 1].prev_block_hash, unconfirmed_blocks[i - 1].data);

        is_correct = is_correct && is_same(unconfirmed_blocks[i].prev_block_hash, evaluated_block_hash);

        // Verify signatures
        is_correct = is_correct && verify_signature(unconfirmed_blocks[i]);
    }

    return is_correct;
}
