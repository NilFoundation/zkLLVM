#![no_main]

use std::intrinsics::assigner_sha2_512;

type BlockType = [__zkllvm_field_pallas_base; 4];

#[circuit]
pub fn sha512_example(
    r: __zkllvm_curve_curve25519,
    pk: __zkllvm_curve_curve25519,
    m: BlockType,
) -> __zkllvm_field_curve25519_scalar {
    assigner_sha2_512(r, pk, m)
}
