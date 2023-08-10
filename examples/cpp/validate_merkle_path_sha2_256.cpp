#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>

using namespace nil::crypto3;

bool is_same(typename hashes::sha2<256>::block_type block0,
    typename hashes::sha2<256>::block_type block1){

    return block0[0] == block1[0] && block0[1] == block1[1];
}

[[circuit]] bool
    validate_path(std::array<typename hashes::sha2<256>::block_type, 0x05> merkle_path,
        typename hashes::sha2<256>::block_type leave,
        typename hashes::sha2<256>::block_type root) {
    
    typename hashes::sha2<256>::block_type subroot = leave;

    for (int i = 0; i < 0x05; i++) {
        subroot = hash<hashes::sha2<256>>(subroot, merkle_path[i]);
    }

    return is_same(subroot, root);
}