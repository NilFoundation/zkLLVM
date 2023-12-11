#ifndef __ZKLLVM__
#include "../../../read_boost_json.hpp"
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] typename pallas::base_field_type::value_type
    pallas_field_add([[private_input]] typename pallas::base_field_type::value_type a,
                                       typename pallas::base_field_type::value_type b) {

    typename pallas::base_field_type::value_type c = a + b;

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

    std::string public_input_name = std::string(argv[1]);
    std::string private_input_name = std::string(argv[1]);

    boost::json::value public_input_json = read_boost_json(public_input_name);

    std::string inputs_str = "/inputs/";
    std::string priv_inp_str = "/private_inputs/";
    size_t char_nr = private_input_name.find(inputs_str);

    ASSERT(char_nr != std::string::npos);
    private_input_name.replace(char_nr, inputs_str.length(), priv_inp_str);

    boost::json::value private_input_json = read_boost_json(private_input_name);


    using BlueprintFieldType = typename pallas::base_field_type;
    typename BlueprintFieldType::value_type a = read_field <BlueprintFieldType>(pubic_input_json, 0);
    typename BlueprintFieldType::value_type b = read_field <BlueprintFieldType>(private_input_json, 0);

    pallas_field_add(a, b);
    return 0;
}
#endif
