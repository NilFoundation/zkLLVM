#![no_main]

use ark_curve25519::{EdwardsAffine, Fr};
use ark_pallas::Fq;

use std::intrinsics::assigner_sha2_512;

type BlockType = [Fq; 4];

#[circuit]
pub fn sha512_example(r: EdwardsAffine, pk: EdwardsAffine, m: BlockType) -> Fr {
    hash(r, pk, m)
}

// This will be arkworks function here in the future.
// All of the implementation details will be hidden from user.
fn hash(r: EdwardsAffine, pk: EdwardsAffine, m: BlockType) -> Fr {
    assigner_sha2_512(r.0, pk.0, [m[0].0, m[1].0, m[2].0, m[3].0]).into()
}
