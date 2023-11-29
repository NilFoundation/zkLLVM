#ifndef __ZKLLVM__
//#include "../read_boost_json.hpp"
#include <cstdint>
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>
#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>


#ifdef __ZKLLVM__

using namespace nil::crypto3;

[[circuit]] typename hashes::sha2<256>::block_type test_func(
    typename hashes::sha2<256>::block_type first_input_block,
    typename hashes::sha2<256>::block_type second_input_block,
    typename hashes::sha2<256>::block_type third_input_block
) {
    typename hashes::sha2<256>::block_type hash_result = hash<hashes::sha2<256>>(first_input_block, second_input_block);
#pragma zk_multi_prover 1
    hash_result = hash<hashes::sha2<256>>(hash_result, third_input_block);

    return hash_result;
}

#endif

#ifndef __ZKLLVM__
int main (int argc, char *argv[]){
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }
    std::cout << "237826622497589691959964175762110627812 311169497251339235781813731085672116686 " << std::endl;
    return 0;
}
#endif
