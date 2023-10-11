#![no_main]

use std::intrinsics::assigner_sha2_256;

use ark_pallas::Fq;

type BlockType = [Fq; 2];

#[circuit]
pub fn sha256_example(first_input_block: BlockType, second_input_block: BlockType) -> BlockType {
    // This will be arkworks function in the future.
    // All of the implementation details will be hidden from user.
    let hash_result = assigner_sha2_256(
        [first_input_block[0].0, first_input_block[1].0],
        [second_input_block[0].0, second_input_block[1].0],
    );

    [hash_result[0].into(), hash_result[1].into()]
}
