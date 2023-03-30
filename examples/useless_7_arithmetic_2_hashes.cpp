// #include <nil/crypto3/algebra/curves/pallas.hpp>

namespace nil {
    namespace crypto3 {
        namespace hashes {
            typedef __attribute__((ext_vector_type(3)))
            __zkllvm_field_pallas_base poseidon_block_type;
            poseidon_block_type poseidon(poseidon_block_type input_block);

			typedef __attribute__((ext_vector_type(2)))
            __zkllvm_field_pallas_base sha256_block_type;
            sha256_block_type sha256(sha256_block_type first_input_block, sha256_block_type second_input_block);

        }    // namespace hashes
    }        // namespace crypto3
}    // namespace nil

[[circuit]] __zkllvm_field_pallas_base field_add(
	__zkllvm_field_pallas_base a_pallas_base,
	__zkllvm_field_pallas_base b_pallas_base,
	__zkllvm_field_pallas_scalar a_pallas_scalar,
	__zkllvm_curve_pallas a_pallas_curve, 
	__zkllvm_curve_pallas b_pallas_curve, 
	__zkllvm_field_curve25519_base a_25519_field, 
	__zkllvm_field_curve25519_base b_25519_field
	) {
	
	typename nil::crypto3::hashes::poseidon_block_type pos_inp = {a_pallas_base, a_pallas_base, a_pallas_base};
    typename nil::crypto3::hashes::poseidon_block_type pos_res;

	typename nil::crypto3::hashes::sha256_block_type sha256_inp1 = {a_pallas_base, a_pallas_base};
	typename nil::crypto3::hashes::sha256_block_type sha256_inp2 = {a_pallas_base, a_pallas_base};
    typename nil::crypto3::hashes::sha256_block_type sha256_res;

	__zkllvm_field_pallas_base res1;
	__zkllvm_field_pallas_base res2;
	__zkllvm_field_pallas_base res3;
	
	__zkllvm_field_curve25519_base variable_25519_add;
	__zkllvm_field_curve25519_base variable_25519_mul;
	__zkllvm_field_curve25519_base variable_25519_sub;

	__zkllvm_curve_pallas variable_curve_add;


	for (int i = 0; i < 3; i++) {
		res1 = a_pallas_base + b_pallas_base;
		res2 = a_pallas_base - b_pallas_base;
		res3 = a_pallas_base * b_pallas_base;

		variable_25519_add = a_25519_field + b_25519_field;
		variable_25519_mul = a_25519_field - b_25519_field;
		variable_25519_sub = a_25519_field * b_25519_field;

		variable_curve_add = a_pallas_curve + b_pallas_curve;

		pos_res = nil::crypto3::hashes::poseidon(pos_inp);
		sha256_res = nil::crypto3::hashes::sha256(sha256_inp1, sha256_inp2);
	}

    return a_pallas_base + a_pallas_base;
}