#![no_main]

use std::intrinsics::assigner_sha2_512;

#[circuit]
pub fn verify_eddsa_signature(
    input_r: __zkllvm_curve_curve25519,
    input_s: __zkllvm_field_curve25519_scalar,
    pk: __zkllvm_curve_curve25519,
    m: [__zkllvm_field_pallas_base; 4],
) -> bool {
    let b = __zkllvm_curve_curve25519::one();
    let k = assigner_sha2_512(input_r, pk, m);
    b * input_s == input_r + (pk * k)
}
