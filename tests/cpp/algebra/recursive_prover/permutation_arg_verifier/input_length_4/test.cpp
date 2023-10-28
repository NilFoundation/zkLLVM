#ifndef __ZKLLVM__
#include "../../../../read_boost_json.hpp"
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

////
#ifdef __ZKLLVM__
typedef __attribute__((ext_vector_type(2)))
                typename pallas::base_field_type::value_type thetas_type;

typedef __attribute__((ext_vector_type(3)))
                typename pallas::base_field_type::value_type output_type;
#else
typedef std::array<typename pallas::base_field_type::value_type, 2> thetas_type;
typedef std::array<typename pallas::base_field_type::value_type, 3> output_type;
#endif

constexpr std::size_t size = 4;

[[circuit]] output_type
    permargver(
        std::array<typename pallas::base_field_type::value_type, size> f,
        std::array<typename pallas::base_field_type::value_type, size> se,
        std::array<typename pallas::base_field_type::value_type, size> sigma,
        typename pallas::base_field_type::value_type L0,
        typename pallas::base_field_type::value_type V,
        typename pallas::base_field_type::value_type V_zeta,
        typename pallas::base_field_type::value_type q_last,
        typename pallas::base_field_type::value_type q_pad,
        thetas_type thetas,
        output_type expected_res
        ) {
    output_type res;

#ifdef __ZKLLVM__
    return __builtin_assigner_permutation_arg_verifier(f.data(), se.data(), sigma.data(), size, L0, V, V_zeta, q_last, q_pad, thetas);
#else
    res = expected_res;
    for (std::size_t i = 0; i < res.size(); i++) {
    std::cout << res[i].data << "\n";
    }
    return res;
#endif
}

#ifndef __ZKLLVM__

int main (int argc, char *argv[]){
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    boost::json::value input_json = read_boost_json(std::string(argv[1]));

    using BlueprintFieldType = typename pallas::base_field_type;

    std::array<typename pallas::base_field_type::value_type, size> f = read_array_field <BlueprintFieldType, size>(input_json, 0);
    std::array<typename pallas::base_field_type::value_type, size> se = read_array_field <BlueprintFieldType, size>(input_json, 1);
    std::array<typename pallas::base_field_type::value_type, size> sigma = read_array_field <BlueprintFieldType, size>(input_json, 2);
    typename pallas::base_field_type::value_type L0 = read_field <BlueprintFieldType>(input_json, 3);
    typename pallas::base_field_type::value_type V = read_field <BlueprintFieldType>(input_json, 4);
    typename pallas::base_field_type::value_type V_zeta = read_field <BlueprintFieldType>(input_json, 5);
    typename pallas::base_field_type::value_type q_last = read_field <BlueprintFieldType>(input_json, 6);
    typename pallas::base_field_type::value_type q_pad = read_field <BlueprintFieldType>(input_json, 7);

    thetas_type thetas = read_vector_field <BlueprintFieldType, thetas.size()>(input_json, 8);
    output_type expected_res = read_vector_field <BlueprintFieldType, expected_res.size()>(input_json, 9);

    permargver(f, se, sigma, L0, V, V_zeta, q_last, q_pad, thetas, expected_res);
    return 0;
}
#endif
