#ifndef __ZKLLVM__
#include "../read_boost_json.hpp"
#include <cstdint>
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

[[circuit]] uint32_t shift_add(uint32_t a) {
    uint32_t out = a >> 1;
#ifdef __ZKLLVM__
#pragma zk_multi_prover 1
    {
        out = out >> 1;
        out = out + 1;
    }
#else
    out = out >> 1;
    out = out + 1;
#endif

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

    uint32_t a = read_uint<uint32_t>(input_json, 0);

    shift_add(a);
    return 0;
}
#endif
