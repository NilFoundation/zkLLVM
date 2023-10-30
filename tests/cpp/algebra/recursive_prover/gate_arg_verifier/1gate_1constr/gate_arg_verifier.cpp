#ifndef __ZKLLVM__
#include "../../../../read_boost_json.hpp"
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;
constexpr std::size_t gates_amount = 1;
constexpr std::size_t constraints_amount = 1;
constexpr std::array<int, gates_amount> gates_sizes = {1};

[[circuit]] typename pallas::base_field_type::value_type
    gate_argument_verifier(
        std::array<typename pallas::base_field_type::value_type, gates_amount> selectors,
        std::array<typename pallas::base_field_type::value_type, constraints_amount> constraints,
        typename pallas::base_field_type::value_type theta,
        typename pallas::base_field_type::value_type expected_res
    ) {

    typename pallas::base_field_type::value_type res;

#ifdef __ZKLLVM__
    res =  __builtin_assigner_gate_arg_verifier( selectors.data(), (int*)gates_sizes.data(), gates_amount, constraints.data(), constraints_amount, theta);
#else
    res = expected_res;
    std::cout << res.data << std::endl;
#endif

    return res;
}


#ifndef __ZKLLVM__

int main (int argc, char *argv[]){
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    boost::json::value input_json = read_boost_json(std::string(argv[1]));

    using BlueprintFieldType = typename pallas::base_field_type;

    std::array<typename pallas::base_field_type::value_type, gates_amount> selectors = read_array_field <BlueprintFieldType, gates_amount>(input_json, 0);
    std::array<typename pallas::base_field_type::value_type, constraints_amount> constraints = read_array_field <BlueprintFieldType, constraints_amount>(input_json, 1);
    typename BlueprintFieldType::value_type theta = read_field <BlueprintFieldType>(input_json, 2);
    typename BlueprintFieldType::value_type expected_res = read_field <BlueprintFieldType>(input_json, 3);

    gate_argument_verifier(selectors, constraints, theta, expected_res);
    return 0;
}
#endif
