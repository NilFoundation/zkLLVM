#![no_main]

use ark_curve25519::EdwardsAffine;

#[circuit]
pub fn add_curves(x: EdwardsAffine, y: EdwardsAffine) -> EdwardsAffine {
    x + y
}
