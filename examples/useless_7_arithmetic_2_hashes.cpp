#include <nil/crypto3/algebra/curves/pallas.hpp>

#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>
#include <nil/crypto3/hash/poseidon.hpp>

using namespace nil::crypto3;

[[circuit]] __zkllvm_field_pallas_base field_add(
	__zkllvm_field_pallas_base a_pallas_base,
	__zkllvm_field_pallas_base b_pallas_base,
	__zkllvm_field_pallas_scalar a_pallas_scalar,
	__zkllvm_curve_pallas a_pallas_curve, 
	__zkllvm_curve_pallas b_pallas_curve, 
	__zkllvm_field_curve25519_base a_25519_field, 
	__zkllvm_field_curve25519_base b_25519_field
	) {
	
    typename algebra::curves::pallas::base_field_type::value_type pos_res;

	typename hashes::sha2<256>::block_type sha256_inp1 = {a_pallas_base, a_pallas_base};
	typename hashes::sha2<256>::block_type sha256_inp2 = {a_pallas_base, a_pallas_base};
    typename hashes::sha2<256>::block_type sha256_res;

	__zkllvm_field_pallas_base res1;
	__zkllvm_field_pallas_base res2;
	__zkllvm_field_pallas_base res3;
	
	__zkllvm_field_curve25519_base variable_25519_add;
	__zkllvm_field_curve25519_base variable_25519_mul;
	__zkllvm_field_curve25519_base variable_25519_sub;

	__zkllvm_curve_pallas variable_curve_add;


	for (int i = 0; i < 10; i++) {
		res1 = a_pallas_base + b_pallas_base;
		res2 = a_pallas_base - b_pallas_base;
		res3 = a_pallas_base * b_pallas_base;

		variable_25519_add = a_25519_field + b_25519_field;
		variable_25519_mul = a_25519_field - b_25519_field;
		variable_25519_sub = a_25519_field * b_25519_field;

		variable_curve_add = a_pallas_curve + b_pallas_curve;

		pos_res = hash<hashes::poseidon>(res1, res2);
		sha256_res = hash<hashes::sha2<256>>(sha256_inp1, sha256_inp2);
        res1 += pos_res;
	}

    return a_pallas_base + a_pallas_base;
}
