// #include <nil/crypto3/algebra/fields/bls12/base_field.hpp>
// #include <nil/crypto3/algebra/curves/bls12.hpp>

// using namespace nil::crypto3;

[[circuit]] __zkllvm_curve_bls12381 verify_signature(
    __zkllvm_field_bls12381_base hashed_msg) {

        // typename algebra::curves::bls12<381>::template g1_type<>::value_type msg_point = __builtin_assigner_hash_to_curve(hashed_msg);
        __zkllvm_curve_bls12381 msg_point = __builtin_assigner_hash_to_curve(hashed_msg);

        return msg_point;
}