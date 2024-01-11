
#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/poseidon.hpp>
#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3;
using namespace nil::crypto3::algebra::curves;

const size_t witness_amount = 15;
const size_t public_input_amount = 1;
const size_t constant_amount = 2;
const size_t selector_amount = 30;
const std::array<std::size_t, public_input_amount> public_input_sizes = {50};
const std::size_t full_public_input_size = 50;

namespace placeholder_verifier{

const bool use_lookups = false;
const size_t batches_num = 4;
const size_t commitments_num = 3;
const size_t points_num = 180;
const size_t poly_num = 107;
const size_t initial_proof_points_num = 1926;
const size_t round_proof_points_num = 72;
const size_t fri_roots_num = 4;
const size_t initial_merkle_proofs_num = 36;
const size_t initial_merkle_proofs_position_num = 54;
const size_t initial_merkle_proofs_hash_num = 216;
const size_t round_merkle_proofs_position_num = 162;
const size_t round_merkle_proofs_hash_num = 162;
const size_t final_polynomial_size = 2;
const size_t lambda = 9;
const size_t rows_amount = 32;
const size_t rows_log = 5;
const size_t total_columns = 48;
const size_t sorted_columns = 0;
const size_t permutation_size = 18;
const std::array<size_t, total_columns> zero_indices = {64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62};
const size_t table_values_num = 80;
const size_t gates_amount = 4;
constexpr std::array<std::size_t, gates_amount> gates_selector_indices = {0, 1, 2, 3};
const size_t constraints_amount = 5;
const size_t quotient_polys_start = 160;
const size_t quotient_polys_amount = 20;
const size_t lookup_sorted_polys_start = 180;
const size_t D0_size = 128;
const size_t D0_log = 7;
const pallas::base_field_type::value_type D0_omega = pallas::base_field_type::value_type(0x1043c1060e0b904d84046899e774702473f9eb889953e9ca842ac3dcb8fba3be_cppui255);
const pallas::base_field_type::value_type omega = pallas::base_field_type::value_type(0x7fc67f0d2530e47f91f36ded523f7e7013069392919e4978abc6e0f780c038c_cppui255);
const size_t fri_rounds = 4;
const std::array<int, gates_amount> gates_sizes = {1, 1, 1, 2};
const size_t unique_points = 4;
const std::array<int, poly_num> point_ids = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const size_t singles_amount = 3;
const std::array<std::size_t, batches_num> batches_amount_list = {70, 16, 1, 20};



struct placeholder_proof_type{
    std::array<pallas::base_field_type::value_type, commitments_num> commitments;
    pallas::base_field_type::value_type challenge;
    std::array<pallas::base_field_type::value_type, points_num> z;
    std::array<pallas::base_field_type::value_type, fri_roots_num> fri_roots;
    std::array<pallas::base_field_type::value_type, initial_proof_points_num> initial_proof_values;
    std::array<pallas::base_field_type::value_type, round_proof_points_num> round_proof_values;
    std::array<int, initial_merkle_proofs_position_num> initial_proof_positions;
    std::array<pallas::base_field_type::value_type, initial_merkle_proofs_hash_num> initial_proof_hashes;
    std::array<int, round_merkle_proofs_position_num> round_merkle_proof_positions;
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
    std::array<pallas::base_field_type::value_type, fri_roots_num> fri_alphas;
    std::array<pallas::base_field_type::value_type, lambda> fri_x_indices;
    pallas::base_field_type::value_type lpc_theta;
    pallas::base_field_type::value_type xi;
};

typedef __attribute__((ext_vector_type(2))) typename pallas::base_field_type::value_type permutation_argument_thetas_type;
typedef __attribute__((ext_vector_type(3))) typename pallas::base_field_type::value_type permutation_argument_output_type;

struct placeholder_permutation_argument_input_type{
    std::array<typename pallas::base_field_type::value_type, permutation_size> xi_values;
    std::array<typename pallas::base_field_type::value_type, permutation_size> id_perm;
    std::array<typename pallas::base_field_type::value_type, permutation_size> sigma_perm;
    permutation_argument_thetas_type thetas;
};

struct transcript_state_type{
    std::array <pallas::base_field_type::value_type, 3> state;
    std::size_t cur;
};

void transcript(transcript_state_type &tr_state, pallas::base_field_type::value_type value) {
    if(tr_state.cur == 3){
        tr_state.state[0] = __builtin_assigner_poseidon_pallas_base({tr_state.state[0],tr_state.state[1],tr_state.state[2]})[2];
        tr_state.state[1] = pallas::base_field_type::value_type(0);
        tr_state.state[2] = pallas::base_field_type::value_type(0);
        tr_state.cur = 1;
    }
	tr_state.state[tr_state.cur] = value;
	tr_state.cur++;
}

pallas::base_field_type::value_type transcript_challenge(transcript_state_type &tr_state) {
    tr_state.state[0] = __builtin_assigner_poseidon_pallas_base({tr_state.state[0], tr_state.state[1], tr_state.state[2]})[2];
    tr_state.state[1] = pallas::base_field_type::value_type(0);
    tr_state.state[2] = pallas::base_field_type::value_type(0);
    tr_state.cur = 1;
    return tr_state.state[0];
}

pallas::base_field_type::value_type pow2_p(pallas::base_field_type::value_type x, size_t plog){
    if(plog == 0) return pallas::base_field_type::value_type(1);
    pallas::base_field_type::value_type result = x;
    for(std::size_t i = 0; i < plog; i++){
        result = result * result;
    }
    return result;
}

pallas::base_field_type::value_type pow2(pallas::base_field_type::value_type x){
    return x*x;
}

pallas::base_field_type::value_type pow3(pallas::base_field_type::value_type x){
    return x*x*x;
}

pallas::base_field_type::value_type pow4(pallas::base_field_type::value_type x){
    pallas::base_field_type::value_type result = x * x;
    result = result * result;
    return result;
}

pallas::base_field_type::value_type pow5(pallas::base_field_type::value_type x){
    pallas::base_field_type::value_type result = x * x;
    result = result * result;
    return result * x;
}

pallas::base_field_type::value_type pow6(pallas::base_field_type::value_type x){
    pallas::base_field_type::value_type result = x * x * x;
    result = result * result;
    return result;
}

pallas::base_field_type::value_type pow7(pallas::base_field_type::value_type x){
    pallas::base_field_type::value_type result = x * x * x;
    result = result * result;
    return result * x;
}

pallas::base_field_type::value_type pow8(pallas::base_field_type::value_type x){
    pallas::base_field_type::value_type result = x * x;
    result = result * result;
    return result * result;
}

pallas::base_field_type::value_type pow9(pallas::base_field_type::value_type x){
    pallas::base_field_type::value_type result = x;
    result = result * result;
    result = result * result;
    result = result * result;
    result = result * x;
    return result;
}

pallas::base_field_type::value_type pow(pallas::base_field_type::value_type x, size_t p){
    pallas::base_field_type::value_type result = 1;
	std::size_t mask = 1;
	while(mask < p){mask = mask * 2;} // 8
 	while(mask > 1){
		result = result * result;
        mask = mask / 2;
		if( p >= mask ){
			result = result * x;
			p = p - mask;
		}
	}
    return result;
}

std::array<pallas::base_field_type::value_type, singles_amount> fill_singles(
    pallas::base_field_type::value_type xi,
    pallas::base_field_type::value_type etha
){
    std::array<pallas::base_field_type::value_type, singles_amount> singles;
	singles[0] = etha;
	singles[1] = xi;
	singles[2] = xi*omega;
;
    return singles;
}

placeholder_challenges_type generate_challenges(
    const std::array<pallas::base_field_type::value_type, 2> &vk,
    const placeholder_proof_type &proof
){
    placeholder_challenges_type challenges;

    transcript_state_type tr_state;
    tr_state.state[0] = pallas::base_field_type::value_type(0);
    tr_state.state[1] = pallas::base_field_type::value_type(0);
    tr_state.state[2] = pallas::base_field_type::value_type(0);
    tr_state.cur = 1;

    transcript(tr_state, vk[0]);
    transcript(tr_state, vk[1]);

    // LPC additional point
    challenges.fri_etha = transcript_challenge(tr_state);

    transcript(tr_state, proof.commitments[0]);

    challenges.perm_beta = transcript_challenge(tr_state);
    challenges.perm_gamma = transcript_challenge(tr_state);

    // Call lookup argument
    if( use_lookups ){
        challenges.lookup_theta = transcript_challenge(tr_state);
        transcript(tr_state, proof.commitments[3]);
        challenges.lookup_beta = transcript_challenge(tr_state);
        challenges.lookup_gamma = transcript_challenge(tr_state);

        for(std::size_t i = 0; i < sorted_columns-1; i++){
            challenges.lookup_alphas[i] = transcript_challenge(tr_state);
        }
    }

    // Call gate argument
    transcript(tr_state, proof.commitments[1]);
    challenges.gate_theta = transcript_challenge(tr_state);

    for(std::size_t i = 0; i < 8; i++){
        challenges.alphas[i] = transcript_challenge(tr_state);
    }
    transcript(tr_state, proof.commitments[2]);

    challenges.xi = transcript_challenge(tr_state);

    transcript(tr_state, vk[1]);
    for(std::size_t i = 0; i < proof.commitments.size(); i++){
        transcript(tr_state, proof.commitments[i]);
    }

    challenges.lpc_theta = transcript_challenge(tr_state);

    for(std::size_t i = 0; i < fri_roots_num; i++){
        transcript(tr_state, proof.fri_roots[i]);
        challenges.fri_alphas[i] = transcript_challenge(tr_state);
    }

    for(std::size_t i = 0; i < lambda; i++){
        challenges.fri_x_indices[i] = transcript_challenge(tr_state);
    }

    return challenges;
}

std::pair<pallas::base_field_type::value_type, pallas::base_field_type::value_type> xi_polys(
    pallas::base_field_type::value_type xi
){
    pallas::base_field_type::value_type xi_n = pow2_p(xi, rows_log) - pallas::base_field_type::value_type(1);
    pallas::base_field_type::value_type l0 = (xi - pallas::base_field_type::value_type(1))*pallas::base_field_type::value_type(rows_amount);
    l0 = xi_n / l0;
    return std::make_pair(l0, xi_n);
}

std::array<pallas::base_field_type::value_type, constraints_amount> calculate_constraints(std::array<pallas::base_field_type::value_type, points_num> z){
    std::array<pallas::base_field_type::value_type, constraints_amount> constraints;
	constraints[0] = ((z[142] + z[143]) - z[144]);
	constraints[1] = (z[142] * z[143] - z[144]);
	constraints[2] = ((z[142] - z[143]) - z[144]);
	constraints[3] = (z[143] * z[144] - z[142]);
	constraints[4] = (z[143] * z[145] - pallas::base_field_type::value_type(0x1_cppui255));


    return constraints;
}



typename pallas::base_field_type::value_type
    gate_argument_verifier(
        std::array<typename pallas::base_field_type::value_type, gates_amount> selectors,
        std::array<typename pallas::base_field_type::value_type, constraints_amount> constraints,
        typename pallas::base_field_type::value_type theta
    ) {

    return __builtin_assigner_gate_arg_verifier(
        selectors.data(),
        (int*)&gates_sizes,
        gates_amount,
        constraints.data(),
        constraints_amount,
        theta
    );
}

std::array<pallas::base_field_type::value_type, 4> getV3(
    pallas::base_field_type::value_type xi0,pallas::base_field_type::value_type xi1,pallas::base_field_type::value_type xi2
){
    std::array<pallas::base_field_type::value_type, 4> result;
    result[0] = - xi0 * xi1 * xi2;
    result[1] = xi0 * xi1  + xi1 * xi2 + xi0 * xi2;
    result[2] = - xi0 - xi1 - xi2;
    result[3] = pallas::base_field_type::value_type(1);
//    __builtin_assigner_exit_check(result[0] + xi0 * result[1] + xi0 * xi0 * result[2] + xi0*xi0*xi0*result[3] == pallas::base_field_type::value_type(0));
    return result;
}

std::array<pallas::base_field_type::value_type, 4> getV2(
    pallas::base_field_type::value_type xi0,pallas::base_field_type::value_type xi1
){
    std::array<pallas::base_field_type::value_type, 4> result;
    result[0] =  xi0 * xi1;
    result[1] = - xi0 - xi1;
    result[2] = pallas::base_field_type::value_type(1);
    result[3] = pallas::base_field_type::value_type(0);
//    __builtin_assigner_exit_check(result[0] + xi0 * result[1] + xi0 * xi0 * result[2] + xi0*xi0*xi0*result[3] == pallas::base_field_type::value_type(0));
    return result;
}

std::array<pallas::base_field_type::value_type, 4> getV1(
    pallas::base_field_type::value_type xi0
){
    std::array<pallas::base_field_type::value_type, 4> result;
    result[0] = - xi0;
    result[1] = pallas::base_field_type::value_type(1);
    result[2] = pallas::base_field_type::value_type(0);
    result[3] = pallas::base_field_type::value_type(0);
//    __builtin_assigner_exit_check(result[0] + xi0 * result[1] + xi0 * xi0 * result[2] + xi0*xi0*xi0*result[3] == pallas::base_field_type::value_type(0));
    return result;
}

std::array<pallas::base_field_type::value_type, 3> getU3(
    pallas::base_field_type::value_type x0,pallas::base_field_type::value_type x1,pallas::base_field_type::value_type x2,
    pallas::base_field_type::value_type z0,pallas::base_field_type::value_type z1,pallas::base_field_type::value_type z2
){
    std::array<pallas::base_field_type::value_type, 3> result;
    pallas::base_field_type::value_type denom = (x0-x1)*(x1-x2)*(x2-x0);

    z0 = z0 * (x2-x1);
    z1 = z1 * (x0-x2);
    z2 = z2 * (x1-x0);

    result[0] = (z0*x1*x2 + z1*x0*x2 + z2*x0*x1)/denom;
    result[1] = (-z0*(x1 + x2) - z1*(x0 + x2) - z2 * (x0 + x1))/denom;
    result[2] = (z0 + z1 + z2)/denom;

//    __builtin_assigner_exit_check(result[0] + x0 * result[1] + x0 * x0 * result[2] == z0/(x2-x1));
//    __builtin_assigner_exit_check(result[0] + x1 * result[1] + x1 * x1 * result[2] == z1/(x0-x2));
//    __builtin_assigner_exit_check(result[0] + x2 * result[1] + x2 * x2 * result[2] == z2/(x1-x0));

    return result;
}

std::array<pallas::base_field_type::value_type, 3> getU2(
    pallas::base_field_type::value_type x0,pallas::base_field_type::value_type x1,
    pallas::base_field_type::value_type z0,pallas::base_field_type::value_type z1
){
    std::array<pallas::base_field_type::value_type, 3> result;
    pallas::base_field_type::value_type denom = (x0-x1);
    result[0] = (-z0*x1 + z1*x0)/denom;
    result[1] = (z0 - z1)/denom;
    result[2] = pallas::base_field_type::value_type(0);

//    __builtin_assigner_exit_check(result[0] + x0 * result[1] + x0 * x0 * result[2] == z0);
//    __builtin_assigner_exit_check(result[0] + x1 * result[1] + x1 * x1 * result[2] == z1);

    return result;
}

std::array<pallas::base_field_type::value_type, 3> getU1(
    pallas::base_field_type::value_type x0,
    pallas::base_field_type::value_type z0
){
    std::array<pallas::base_field_type::value_type, 3> result;
    result[0] = z0;
    result[1] = pallas::base_field_type::value_type(0);
    result[2] = pallas::base_field_type::value_type(0);

//    __builtin_assigner_exit_check(result[0] + x0 * result[1] + x0 * x0 * result[2] == z0);

    return result;
}

pallas::base_field_type::value_type eval4(std::array<pallas::base_field_type::value_type, 4> poly, pallas::base_field_type::value_type x){
    pallas::base_field_type::value_type result;
    result = poly[3];
    result = result *x + poly[2];
    result = result *x + poly[1];
    result = result *x + poly[0];
//    __builtin_assigner_exit_check(poly[0] + x * poly[1] + x * x * poly[2] + x*x*x*poly[3] == result);
    return result;
}

pallas::base_field_type::value_type eval3(std::array<pallas::base_field_type::value_type, 3> poly, pallas::base_field_type::value_type x){
    pallas::base_field_type::value_type result;
    result = poly[2];
    result = result *x + poly[1];
    result = result *x + poly[0];
//    __builtin_assigner_exit_check(poly[0] + x * poly[1] + x * x * poly[2] == result);
    return result;
}

pallas::base_field_type::value_type calculate_leaf_hash(
    std::array<pallas::base_field_type::value_type, initial_proof_points_num> val,
    std::size_t start_index,
    std::size_t leaf_size
){
    pallas::base_field_type::value_type hash_state = pallas::base_field_type::value_type(0);
    for(std::size_t pos = 0; pos < leaf_size; pos+=2){
        hash_state = __builtin_assigner_poseidon_pallas_base(
            {hash_state, val[start_index + pos], val[start_index + pos+1]}
        )[2];
    }
    return hash_state;
}

pallas::base_field_type::value_type calculate_reversed_leaf_hash(
    std::array<pallas::base_field_type::value_type, initial_proof_points_num> &val,
    std::size_t start_index,
    std::size_t leaf_size
){
    pallas::base_field_type::value_type hash_state = pallas::base_field_type::value_type(0);
    for(std::size_t pos = 0; pos < leaf_size; pos+=2){
        hash_state = __builtin_assigner_poseidon_pallas_base(
            {hash_state, val[start_index + pos + 1], val[start_index + pos]}
        )[2];
    }
    return hash_state;
}
struct precomputed_values_type{
    pallas::base_field_type::value_type l0;
    pallas::base_field_type::value_type Z_at_xi;
    pallas::base_field_type::value_type F_consolidated;
    pallas::base_field_type::value_type T_consolidated;
};

constexpr std::size_t L0_IND = 0;
constexpr std::size_t Z_AT_XI_IND = 1;
constexpr std::size_t F_CONSOLIDATED_IND = 2;
constexpr std::size_t T_CONSOLIDATED_IND = 3;

typedef __attribute__((ext_vector_type(2)))
                typename pallas::base_field_type::value_type pair_type;

typedef __attribute__((ext_vector_type(4)))
                typename pallas::base_field_type::value_type lookup_output_type;

typedef __attribute__((ext_vector_type(2)))
                typename pallas::base_field_type::value_type pair_type;


[[circuit]] bool placeholder_verifier(
    std::array<pallas::base_field_type::value_type, full_public_input_size> public_input,
    std::array<pallas::base_field_type::value_type, 2> vk,
    placeholder_proof_type proof
) {
    return true;
}

}
