#ifndef __ZKLLVM__
#include "../../../read_boost_json.hpp"
#include <fstream>
#endif

#include <nil/crypto3/algebra/fields/curve25519/base_field.hpp>
#include <nil/crypto3/algebra/curves/ed25519.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] typename ed25519::base_field_type::value_type field_addition(
        typename ed25519::base_field_type::value_type a,
        typename ed25519::base_field_type::value_type b) {

    typename ed25519::base_field_type::value_type c = a + b;

    #ifndef __ZKLLVM__
    std::cout << c.data <<std::endl;
    #endif

    return c;
}

#ifndef __ZKLLVM__

int main (int argc, char *argv[]){
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    boost::json::value input_json = read_boost_json(std::string(argv[1]));

    using OperatingFieldType = typename ed25519::base_field_type;
    typename OperatingFieldType::value_type a = read_field <OperatingFieldType>(input_json, 0);
    typename OperatingFieldType::value_type b = read_field <OperatingFieldType>(input_json, 1);

    field_addition(a, b);
    return 0;
}
#endif
