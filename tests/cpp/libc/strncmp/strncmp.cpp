#ifndef __ZKLLVM__
#include "../../read_boost_json.hpp"
#include <cstdint>
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

[[circuit]] int test_func(const char *s1, const char *s2, uint32_t n) {
    int out = strncmp(s1, s2, n);

#ifndef __ZKLLVM__
    std::cout << out <<std::endl;
#endif

    return out;
}

#ifndef __ZKLLVM__

int main (int argc, char *argv[]){
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    boost::json::value input_json = read_boost_json(std::string(argv[1]));

    std::string s1 = read_string(input_json, 0);
    std::string s2 = read_string(input_json, 1);
    uint32_t n = read_uint<uint32_t>(input_json, 2);


    test_func(s1.c_str(), s2.c_str(), n);

    return 0;
}
#endif
