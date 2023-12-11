#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

constexpr std::size_t gates_amount = 4;
constexpr std::size_t constraints_amount = 10;
constexpr std::array<int, gates_amount> gates_sizes = {3,2,4,1};

[[circuit]] typename pallas::base_field_type::value_type
    gate_argument_verifier(
        std::array<typename pallas::base_field_type::value_type, gates_amount> selectors,
        std::array<typename pallas::base_field_type::value_type, constraints_amount> constraints,
        typename pallas::base_field_type::value_type theta
    ) {

    return __builtin_assigner_gate_arg_verifier( selectors.data(), (int*)&gates_sizes, gates_amount, constraints.data(), constraints_amount, theta);

}
