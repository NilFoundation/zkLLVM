namespace nil {
    namespace crypto3 {
        namespace hashes {
            __zkllvm_curve_bls12381 hash_to_curve(__zkllvm_field_bls12381_base a); //to curve
        }    // namespace hashes
    }        // namespace crypto3
}    // namespace nil

namespace nil {
    namespace crypto3 {
        namespace algebra {
            typedef __attribute__((ext_vector_type(4)))
            __zkllvm_field_bls12381_base g2_type;

            typedef __attribute__((ext_vector_type(12)))
            __zkllvm_field_bls12381_base gT_type;

            gT_type optimal_ate_pairing(__zkllvm_curve_bls12381 a, g2_type b);
            __zkllvm_field_bls12381_base check_equality(gT_type a, gT_type b);
        }    // namespace hashes
    }        // namespace crypto3
}    // namespace nil

[[circuit]] __zkllvm_field_bls12381_base verify_signature(
    __zkllvm_field_bls12381_base hashed_msg,
    typename nil::crypto3::algebra::g2_type pubkey,
    __zkllvm_curve_bls12381 sig) {
        __zkllvm_curve_bls12381 msg_point = nil::crypto3::hashes::hash_to_curve(hashed_msg);

        typename nil::crypto3::algebra::g2_type g2 = pubkey; // TODO replace with real g2

        typename nil::crypto3::algebra::gT_type pairing1 = nil::crypto3::algebra::optimal_ate_pairing(sig, g2);
        typename nil::crypto3::algebra::gT_type pairing2 = nil::crypto3::algebra::optimal_ate_pairing(msg_point, pubkey);

        return nil::crypto3::algebra::check_equality(pairing1, pairing2);
}