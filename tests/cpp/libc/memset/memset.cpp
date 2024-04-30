#ifndef __ZKLLVM__
#include "../../read_boost_json.hpp"
#include <cstdint>
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

[[circuit]] uint32_t test_func(char *buf) {
    uint32_t out = 0;
    memset((void *)buf, 5, 2);
    out = buf[0] + buf[1];

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

    char *buf = new char[s.size()];
    strcpy(buf, s.c_str());

    test_func(buf);

    delete[] buf;
    return 0;
}
#endif
