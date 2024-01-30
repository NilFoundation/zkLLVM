#![no_main]
#![feature(stmt_expr_attributes)]

// In this example built-in field type is used.
type PallasBase = __zkllvm_field_pallas_base;

use unroll::unroll_for_loops;

#[unroll_for_loops]
fn pow_2(a: PallasBase) -> PallasBase {
    let mut acc = 1g;
    for i in 0..2 {
        acc = acc * a;
    }

    acc
}

#[circuit]
pub fn field_arithmetic_example(a: PallasBase, b: PallasBase) -> PallasBase {
    let c = (a + b) * a + b * (a + b) * (a + b);
    const CONSTANT: PallasBase = 0x12345678901234567890g;
    c * c * c / (b - a) + pow_2(a) + CONSTANT
}
