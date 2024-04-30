#ifndef __ZKLLVM__
#include "../../read_boost_json.hpp"
#include <cstdint>
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

[[circuit]] uint32_t test_func(char *dst, const char *src, uint32_t n) {
    uint32_t out = 0;
    char *ch = strncpy(dst, src, n);
    out = ch[0] + ch[1];

#ifndef __ZKLLVM__
    std::cout << out << std::endl;
#endif

    return out;
}

#ifndef __ZKLLVM__

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    boost::json::value input_json = read_boost_json(std::string(argv[1]));

    std::string s = read_string(input_json, 0);
    uint32_t n = read_uint<uint32_t>(input_json, 2);

    char *buf = new char[s.size()];
    memset((void *)buf, 0, s.size());

    test_func(buf, s.c_str(), n);

    delete[] buf;
    return 0;
}
#endif
