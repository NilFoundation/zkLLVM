#![no_main]

use std::intrinsics::{assigner_exit_check, assigner_sha2_256};

use ark_pallas::Fq;

use unroll::unroll_for_loops;

type Block = [Fq; 2];

#[circuit]
#[unroll_for_loops]
pub fn validate_path(merkle_path: [Block; 5], leave: Block, root: Block) {
    // TODO: (aleasims) use `fold` here instead of explicit iteration.
    // Right now assigner fails with `fold`.
    let mut subroot = leave;
    for i in 0..5 {
        subroot = hash(subroot, merkle_path[i]);
    }
    unsafe { assigner_exit_check(equal(subroot, root)) }
}

// TODO: (aleasims) remove this function and simply use `==` instead.
// Right now assigner fails if `==` is used.
fn equal(x: Block, y: Block) -> bool {
    x[0] == y[0] && x[1] == y[1]
}

// This will be SDK function in the future.
// All of the implementation details will be hidden from user.
fn hash(block1: Block, block2: Block) -> Block {
    let sha = assigner_sha2_256([block1[0].0, block1[1].0], [block2[0].0, block2[1].0]);
    [sha[0].into(), sha[1].into()]
}
