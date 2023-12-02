#ifndef __ZKLLVM__
#include "../../read_boost_json.hpp"
#include <cstdint>
#include <fstream>
#endif

#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>

#ifdef __ZKLLVM__

using namespace nil::crypto3;

[[circuit]] typename hashes::sha2<256>::block_type test_func(typename hashes::sha2<256>::block_type first_input_block,
                                                             typename hashes::sha2<256>::block_type second_input_block) {
    typename hashes::sha2<256>::block_type hash_result = hash<hashes::sha2<256>>(first_input_block, second_input_block);

    return hash_result;
}

#endif

#ifndef __ZKLLVM__

int main (int argc, char *argv[]){
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    std::cout << "88644561790884159333416244513756961366" << std::endl;
    std::cout << "279256413903306112288588927002886743215" << std::endl;

    return 0;
}
#endif
