#![no_main]

use ark_curve25519::{EdwardsAffine, Fr};
use ark_pallas::Fq;

use std::intrinsics::assigner_sha2_512;

type EdDSAMessageBlockType = [Fq; 4];

#[circuit]
pub fn verify_eddsa_signature(
    input_r: EdwardsAffine,
    input_s: Fr,
    pk: EdwardsAffine,
    m: EdDSAMessageBlockType,
) -> bool {
    let b = EdwardsAffine::one(); // here must be `EdwardsAffine::gerenator()`
    let k = hash(input_r, pk, m);
    b * input_s == input_r + (pk * k)
}

// This will be arkworks function here in the future.
// All of the implementation details will be hidden from user.
fn hash(r: EdwardsAffine, pk: EdwardsAffine, m: EdDSAMessageBlockType) -> Fr {
    assigner_sha2_512(r.0, pk.0, [m[0].0, m[1].0, m[2].0, m[3].0]).into()
}
