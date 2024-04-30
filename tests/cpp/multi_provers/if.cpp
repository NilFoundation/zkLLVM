#ifndef __ZKLLVM__
#include "../read_boost_json.hpp"
#include <cstdint>
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3;

void f(uint32_t* out) {
    if (out[0] == 0) {
        out[0] = 1;
    }
    out[0] *= 2;
}

[[circuit]] uint32_t test(uint32_t a) {
    uint32_t out[2] = {0, 0};
    if (a > 10) {
        bool same = (a == 11);
        f(out);
        for (uint32_t i = 0; i < 5; i++) {
            out[0] = out[0] + 1;
        }
#ifdef __ZKLLVM__
#pragma zk_multi_prover 1
#endif
        uint32_t tmp = a * 2;
        out[0] = same ? out[0] : tmp;
    } else {
        out[0] = a;
    }
    out[1] = a;
    return out[0] + out[1];
}

#ifndef __ZKLLVM__

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    boost::json::value input_json = read_boost_json(std::string(argv[1]));

    uint32_t a = read_uint<uint32_t>(input_json, 0);

    std::cout << test(a) << std::endl;
    return 0;
}
#endif
