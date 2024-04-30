#ifndef __ZKLLVM__
#include "../../read_boost_json.hpp"
#include <cstdint>
#include <fstream>
#endif

#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/poseidon.hpp>

#ifdef __ZKLLVM__

using namespace nil::crypto3;
using namespace nil::crypto3::algebra::curves;

[[circuit]] typename pallas::base_field_type::value_type test_func(typename pallas::base_field_type::value_type b,
                                                                   typename pallas::base_field_type::value_type c) {
    typename pallas::base_field_type::value_type hash_result = hash<hashes::poseidon>(b, c);

    return hash_result;
}

#endif

#ifndef __ZKLLVM__

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    std::cout << "1090649087601646060577911565538921894952249843885775285625455432992693168204" << std::endl;

    return 0;
}
#endif
