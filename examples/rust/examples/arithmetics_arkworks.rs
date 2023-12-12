#![no_main]
#![feature(const_trait_impl)]
#![feature(effects)]

use ark_ff::{Field, MontFp};
use ark_pallas::Fq;

fn pow(a: Fq, n: u32) -> Fq {
    (0..n).fold(Fq::ONE, |acc, _| acc * a)
}

#[circuit]
pub fn field_arithmetic_example(a: Fq, b: Fq) -> Fq {
    let c = (a + b) * a + b * (a + b) * (a + b);
    const CONSTANT: Fq = MontFp!("0x12345678901234567890");
    c * c * c / (b - a) + pow(a, 2) + CONSTANT
}
