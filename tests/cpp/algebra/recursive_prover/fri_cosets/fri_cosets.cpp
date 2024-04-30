#ifndef __ZKLLVM__
#include "../../../read_boost_json.hpp"
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

constexpr typename pallas::base_field_type::value_type omega =
    0x1ff2863fd35bfc59e51f3693bf37e2d841d1b5fbed4138f755a638bec8750abd_cppui255;
constexpr size_t n = 4;

#ifndef __ZKLLVM__
void calculate_expected_res(std::array<std::array<typename pallas::base_field_type::value_type, 3>, n> &res) {
    typename pallas::base_field_type::integral_type pi_num = 0xa53a16c34fb833b5_cppui255;
    typename pallas::base_field_type::value_type w_powers = omega;
    typename pallas::base_field_type::value_type w_pow_x = 1;

    for (std::size_t i = 0; i < n; i++) {
        w_pow_x *= (pi_num % 2 == 1) ? w_powers : 1;
        // (n-1-i) because the bits in the result are in reverse order
        res[n - 1 - i][2] = typename pallas::base_field_type::value_type(pi_num % 2);
        pi_num /= 2;
        w_powers *= w_powers;
    }
    for (std::size_t i = 0; i < n; i++) {
        res[i][0] = w_pow_x;
        res[i][1] = res[i][0] * (-1);
        w_pow_x *= w_pow_x;
    }
}
#endif

[[circuit]] std::array<std::array<typename pallas::base_field_type::value_type, 3>, n>
    fri_cosets_test(typename pallas::base_field_type::value_type input) {

    std::array<std::array<typename pallas::base_field_type::value_type, 3>, n> res;

#ifdef __ZKLLVM__
    __builtin_assigner_fri_cosets(res.data(), n, omega, input);

    for (std::size_t i = 0; i < n; i++) {
        for (std::size_t j = 0; j < 3; j++) {
            __builtin_assigner_print_native_pallas_field(res[i][j]);
        }
    }

#else
    calculate_expected_res(res);

    for (std::size_t i = 0; i < n; i++) {
        for (std::size_t j = 0; j < 3; j++) {
            std::cout << res[i][j].data << std::endl;
        }
    }

#endif

    return res;
}

#ifndef __ZKLLVM__

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    boost::json::value input_json = read_boost_json(std::string(argv[1]));

    using BlueprintFieldType = typename pallas::base_field_type;
    typename BlueprintFieldType::value_type input = read_field<BlueprintFieldType>(input_json, 0);

    fri_cosets_test(input);
    return 0;
}
#endif
