#![no_main]

use ark_pallas::Fq;

fn pow(a: Fq, n: u32) -> Fq {
    (0..n).fold(1g.into(), |acc, _| acc * a)
}

#[circuit]
pub fn field_arithmetic_example(a: Fq, b: Fq) -> Fq {
    let c = (a + b) * a + b * (a + b) * (a + b);
    const CONSTANT: Fq = Fq(0x12345678901234567890g);
    c * c * c / (b - a) + pow(a, 2) + CONSTANT
}
