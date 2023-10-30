#ifndef __ZKLLVM__
#include "../../../read_boost_json.hpp"
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] typename pallas::base_field_type::value_type
    fri_lin_inter(
        typename pallas::base_field_type::value_type s,
        typename pallas::base_field_type::value_type y0,
        typename pallas::base_field_type::value_type y1,
        typename pallas::base_field_type::value_type alpha
    ) {

    typename pallas::base_field_type::value_type expected_res;


#ifdef __ZKLLVM__
    expected_res = __builtin_assigner_fri_lin_inter(s, y0, y1, alpha);
#else
    expected_res = y0 + (y1 - y0) * (s - alpha) / (2 * s);
    std::cout << expected_res.data <<std::endl;
#endif

    return expected_res;
}

#ifndef __ZKLLVM__

int main (int argc, char *argv[]){
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    boost::json::value input_json = read_boost_json(std::string(argv[1]));

    using BlueprintFieldType = typename pallas::base_field_type;
    typename BlueprintFieldType::value_type s = read_field <BlueprintFieldType>(input_json, 0);
    typename BlueprintFieldType::value_type y0 = read_field <BlueprintFieldType>(input_json, 1);
    typename BlueprintFieldType::value_type y1 = read_field <BlueprintFieldType>(input_json, 2);
    typename BlueprintFieldType::value_type alpha = read_field <BlueprintFieldType>(input_json, 3);

    fri_lin_inter(s, y0, y1, alpha);
    return 0;
}
#endif
