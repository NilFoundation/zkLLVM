#ifndef __ZKLLVM__
#include "../../../read_boost_json.hpp"
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] bool equal(typename pallas::base_field_type::value_type a, typename pallas::base_field_type::value_type b) {

    bool c = a == b;

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

    using OperatingFieldType = typename pallas::base_field_type;
    typename OperatingFieldType::value_type a = read_field<OperatingFieldType>(input_json, 0);
    typename OperatingFieldType::value_type b = read_field<OperatingFieldType>(input_json, 1);

    equal(a, b);
    return 0;
}
#endif
