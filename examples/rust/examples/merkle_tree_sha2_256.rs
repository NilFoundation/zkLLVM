#![no_main]

use std::intrinsics::assigner_sha2_256;

use ark_pallas::Fq;

type BlockType = [Fq; 2];

#[circuit]
pub fn balance(layer_0_leaves: [BlockType; 0x10]) -> BlockType {
    let mut layer_1_leaves: [BlockType; 0x8] = [[0g.into(), 0g.into()]; 0x8];
    let layer_1_size = 0x8;
    let mut layer_2_leaves: [BlockType; 0x4] = [[0g.into(), 0g.into()]; 0x4];
    let layer_2_size = 0x4;
    let mut layer_3_leaves: [BlockType; 0x2] = [[0g.into(), 0g.into()]; 0x2];
    let layer_3_size = 0x2;

    for leaf_index in 0..layer_1_size {
        layer_1_leaves[leaf_index] = hash(layer_0_leaves[2 * leaf_index], layer_0_leaves[2 * leaf_index + 1]);
    }

    for leaf_index in 0..layer_2_size {
        layer_2_leaves[leaf_index] = hash(layer_1_leaves[2 * leaf_index], layer_1_leaves[2 * leaf_index + 1]);
    }

    for leaf_index in 0..layer_3_size {
        layer_3_leaves[leaf_index] = hash(layer_2_leaves[2 * leaf_index], layer_2_leaves[2 * leaf_index + 1]);
    }

    let root: BlockType = hash(layer_3_leaves[0], layer_3_leaves[1]);

    root
}

// This will be arkworks function in the future.
// All of the implementation details will be hidden from user.
fn hash(block1: BlockType, block2: BlockType) -> BlockType {
    let sha = assigner_sha2_256([block1[0].0, block1[1].0], [block2[0].0, block2[1].0]);
    [sha[0].into(), sha[1].into()]
}
