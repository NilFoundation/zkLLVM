#ifndef __ZKLLVM__
#include "../../read_boost_json.hpp"
#include <cstdint>
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] bool assert_equaluty(typename pallas::base_field_type::value_type a,
                                 typename pallas::base_field_type::value_type b) {

    bool c = a == b;

#ifdef __ZKLLVM__
    __builtin_assigner_exit_check(c);
#else
    assert(c);
#endif

#ifndef __ZKLLVM__
    std::cout << c << std::endl;
#endif

    return c;
}

#ifndef __ZKLLVM__

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    boost::json::value input_json = read_boost_json(std::string(argv[1]));

    using BlueprintFieldType = typename pallas::base_field_type;
    typename BlueprintFieldType::value_type a = read_field<BlueprintFieldType>(input_json, 0);
    typename BlueprintFieldType::value_type b = read_field<BlueprintFieldType>(input_json, 1);

    assert_equaluty(a, b);
    return 0;
}
#endif
