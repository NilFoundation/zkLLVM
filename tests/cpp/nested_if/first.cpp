#ifndef __ZKLLVM__
#include "../read_boost_json.hpp"
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] typename pallas::base_field_type::value_type
    pallas_field_add(typename pallas::base_field_type::value_type a, typename pallas::base_field_type::value_type b) {

#ifdef __ZKLLVM__
    if (a > 2) {
        if (b > 2) {
            return a + b;
        } else {
            return a - b;
        }
    }
    if (a < 2) {
        if (b < 2) {
            return a * b;
        } else {
            return a / b;
        }
    }
    return 0;
#else

    typename pallas::base_field_type::value_type c;

    if (a > 2) {
        if (b > 2) {
            c = a + b;
            std::cout << c.data << std::endl;
            return c;
        } else {
            c = a - b;
            std::cout << c.data << std::endl;
            return c;
        }
    }
    if (a < 2) {
        if (b < 2) {
            c = a * b;
            std::cout << c.data << std::endl;
            return c;
        } else {
            c = a / b;
            std::cout << c.data << std::endl;
            return c;
        }
    }

    c = typename pallas::base_field_type::value_type(0);
    std::cout << c.data << std::endl;
    return c;
#endif
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

    pallas_field_add(a, b);
    return 0;
}
#endif
