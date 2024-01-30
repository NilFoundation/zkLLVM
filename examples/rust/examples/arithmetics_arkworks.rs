#![no_main]
#![feature(const_trait_impl)]
#![feature(effects)]

use ark_ff::{Field, MontFp};
use ark_pallas::Fq;
use unroll::unroll_for_loops;

#[unroll_for_loops]
fn pow_2(a: Fq) -> Fq {
    let mut acc = Fq::ONE;
    for i in 0..2 {
        acc = acc * a;
    }

    acc
}

#[circuit]
pub fn field_arithmetic_example(a: Fq, b: Fq) -> Fq {
    let c = (a + b) * a + b * (a + b) * (a + b);
    const CONSTANT: Fq = MontFp!("0x12345678901234567890");
    c * c * c / (b - a) + pow_2(a) + CONSTANT
}
