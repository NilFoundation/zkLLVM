#![no_main]

// In this example built-in field type is used.
type PallasBase = __zkllvm_field_pallas_base;

fn fib100(a: PallasBase, b: PallasBase) -> PallasBase {
    let mut prev_target = a;
    let mut cur_target = b;

    for _ in 0..99 {
        let temp = prev_target + cur_target;
        prev_target = cur_target;
        cur_target = temp;
    }

    cur_target
}

#[circuit]
pub fn verify_f100(a: PallasBase, b: PallasBase, f100: PallasBase) -> bool {
    fib100(a,b) == f100
}
