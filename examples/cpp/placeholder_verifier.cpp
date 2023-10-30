
#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/poseidon.hpp>
#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3;
using namespace nil::crypto3::algebra::curves;

const bool use_lookups = false;
const size_t batches_num = 4;
const size_t commitments_num = 3;
const size_t points_num = 210;
const size_t poly_num = 123;
const size_t initial_proof_points_num = 492;
const size_t round_proof_points_num = 20;
const size_t fri_roots_num = 5;
const size_t initial_merkle_proofs_num = 8;
const size_t initial_merkle_proofs_position_num = 12;
const size_t initial_merkle_proofs_hash_num = 48;
const size_t round_merkle_proofs_position_num = 40;
const size_t round_merkle_proofs_hash_num = 40;
const size_t final_polynomial_size = 2;

struct placeholder_proof_type{
    std::array<pallas::base_field_type::value_type, commitments_num> commitments;
    pallas::base_field_type::value_type challenge;
    std::array<pallas::base_field_type::value_type, points_num> z;
    std::array<pallas::base_field_type::value_type, fri_roots_num> fri_roots;
    std::array<pallas::base_field_type::value_type, initial_proof_points_num> initial_proof_values;
    std::array<pallas::base_field_type::value_type, round_proof_points_num> round_proof_values;
    std::array<pallas::base_field_type::value_type, initial_merkle_proofs_position_num> initial_proof_positions;
    std::array<pallas::base_field_type::value_type, initial_merkle_proofs_hash_num> initial_proof_hashes;
    std::array<pallas::base_field_type::value_type, round_merkle_proofs_position_num> round_merkle_proof_positions;
    std::array<pallas::base_field_type::value_type, round_merkle_proofs_hash_num> round_proof_hashes;
    std::array<pallas::base_field_type::value_type, final_polynomial_size> final_polynomial;
};

struct placeholder_challenges_type{
    pallas::base_field_type::value_type fri_etha;
    pallas::base_field_type::value_type perm_beta;
    pallas::base_field_type::value_type perm_gamma;
    pallas::base_field_type::value_type lookup_theta;
    pallas::base_field_type::value_type lookup_gamma;
    pallas::base_field_type::value_type lookup_beta;
    std::array<pallas::base_field_type::value_type, 1> lookup_alphas;
    pallas::base_field_type::value_type gate_theta;
    std::array<pallas::base_field_type::value_type, 8> alphas;
    std::array<pallas::base_field_type::value_type, 8> fri_alphas;
    std::array<pallas::base_field_type::value_type, 8> fri_x_indices;
    pallas::base_field_type::value_type xi;
};

pallas::base_field_type::value_type transcript(pallas::base_field_type::value_type tr_state, pallas::base_field_type::value_type value) {
    return hash<hashes::poseidon>(value, hash<hashes::poseidon>(tr_state, tr_state));
}

std::pair<pallas::base_field_type::value_type, pallas::base_field_type::value_type > transcript_challenge(pallas::base_field_type::value_type tr_state) {
    return std::make_pair(hash<hashes::poseidon>(tr_state, tr_state), hash<hashes::poseidon>(tr_state, tr_state));
}

[[circuit]] bool placeholder_verifier(
    std::array<pallas::base_field_type::value_type, 2> vk,
    placeholder_proof_type proof
) {
    placeholder_challenges_type challenges;

    pallas::base_field_type::value_type tr_state(0x2fadbe2852044d028597455bc2abbd1bc873af205dfabb8a304600f3e09eeba8_cppui255);

    tr_state = transcript(tr_state, vk[0]);
    tr_state = transcript(tr_state, vk[1]);

    // LPC additional point
    std::tie(tr_state, challenges.fri_etha) = transcript_challenge(tr_state);

    tr_state = transcript(tr_state, proof.commitments[0]);

    std::tie(tr_state, challenges.perm_beta) = transcript_challenge(tr_state);
    std::tie(tr_state, challenges.perm_gamma) = transcript_challenge(tr_state);

    // Call permutation argument

    // Call lookup argument
    if( use_lookups ){
        __builtin_assigner_exit_check(false);
    }

    // Call gate argument
    tr_state = transcript(tr_state, proof.commitments[1]);
    std::tie(tr_state, challenges.gate_theta) = transcript_challenge(tr_state);

    for(std::size_t i = 0; i < 8; i++){
        std::tie(tr_state, challenges.alphas[i]) = transcript_challenge(tr_state);
    }
    tr_state = transcript(tr_state, proof.commitments[2]);

    std::tie(tr_state, challenges.xi) = transcript_challenge(tr_state);

    for(std::size_t i = 0; i < fri_roots_num; i++){
        tr_state = transcript(tr_state, proof.fri_roots[i]);
        challenges.fri_alphas[i] = transcript(tr_state, proof.fri_roots[i]);
    }

    __builtin_assigner_exit_check(challenges.xi == proof.challenge);

    return true;
}
