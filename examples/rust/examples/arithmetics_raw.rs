#![no_main]
#![no_std]

// In this example built-in field type is used.
type PallasBase = __zkllvm_field_pallas_base;

fn pow(a: PallasBase, n: u32) -> PallasBase {
    if n == 0 {
        return 1g;
    }
    let mut res = 1g;
    let mut i = 0;
    while i < n {
        res *= a;
        i += 1;
    }
    res
}

#[circuit]
pub fn field_arithmetic_example(a: PallasBase, b: PallasBase) -> PallasBase {
    let c = (a + b) * a + b * (a + b) * (a + b);
    const CONSTANT: PallasBase = 0x12345678901234567890g;
    c * c * c / (b - a) + pow(a, 2) + CONSTANT
}
