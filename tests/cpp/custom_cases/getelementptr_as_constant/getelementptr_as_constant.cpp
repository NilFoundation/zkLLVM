#ifndef __ZKLLVM__
#include "../../read_boost_json.hpp"
#include <cstdint>
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

struct lumpinfo_s {
    lumpinfo_s *next;
    lumpinfo_s *prev;
    int data1;
};

lumpinfo_s first;

[[circuit]] int list_demo(int unused) {

    first.next = first.prev = &first;

#ifndef __ZKLLVM__
    std::cout << 0 << std::endl;
#endif

    return 0;
}

#ifndef __ZKLLVM__

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    boost::json::value input_json = read_boost_json(std::string(argv[1]));

    int a = read_uint<int>(input_json, 0);

    list_demo(a);
    return 0;
}
#endif
