#ifndef __ZKLLVM__
#include "../../../read_boost_json.hpp"
#include <cstdint>
#include <iostream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

using field_type = nil::crypto3::algebra::curves::pallas::base_field_type::value_type;
[[circuit]] field_type test_func(uint64_t lower, uint64_t higher) {
    field_type result = higher;
    result *= (1L << 32);
    result *= (1L << 32);
    result += lower;
    return result;
}

#ifndef __ZKLLVM__

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    boost::json::value input_json = read_boost_json(std::string(argv[1]));

    uint64_t lower = read_uint<uint64_t>(input_json, 0);
    uint64_t higher = read_uint<uint64_t>(input_json, 1);

    std::cout << test_func(lower, higher) << std::endl;

    return 0;
}
#endif
