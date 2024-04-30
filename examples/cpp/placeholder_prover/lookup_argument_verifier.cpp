#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

#define ARRAY_SUM_3(arr) arr[0] + arr[1] + arr[2]
#define ARRAY_SUM_4(arr) arr[0] + arr[1] + arr[2] + arr[3]
#define ARRAY_SUM_6(arr) arr[0] + arr[1] + arr[2] + arr[3] + arr[4] + arr[5]
#define ARRAY_SCALAR_MUL_4(arr1, arr2) arr1[0] * arr2[0] + arr1[1] * arr2[1] + arr1[2] * arr2[2] + arr1[3] * arr2[3]

constexpr std::size_t lookup_table_size = 4;
constexpr std::size_t lookup_gate_size = 3;

// I'll take example with different sizes to prevent unexpected dependencies
// All of these is characteristics of each lookup table
constexpr std::array<std::size_t, lookup_table_size> lookup_table_lookup_options_sizes = {2, 1, 1, 3};
constexpr std::array<std::size_t, lookup_table_size> lookup_table_columns_numbers = {3, 2, 1, 2};
// In this case it'll be 7 = 2+1+1+3 options
constexpr std::size_t lookup_options_size = ARRAY_SUM_4(lookup_table_lookup_options_sizes);
constexpr std::size_t lookup_value_columns_size =
    ARRAY_SCALAR_MUL_4(lookup_table_lookup_options_sizes, lookup_table_columns_numbers);

constexpr std::array<std::size_t, lookup_gate_size> lookup_gate_constraints_sizes = {1, 2, 3};
// In this case there are 6 = 1 + 2 + 3 lookup constraints
constexpr std::size_t lookup_constraints_size = ARRAY_SUM_3(lookup_gate_constraints_sizes);
constexpr std::array<std::size_t, lookup_constraints_size> lookup_gate_constraints_lookup_input_sizes = {2, 1, 1,
                                                                                                         3, 2, 1};
// In this case 10 = 2 + 1 + 1+ 3 + 2 + 1
constexpr std::size_t lookup_input_columns_size = ARRAY_SUM_6(lookup_gate_constraints_lookup_input_sizes);

// In this case 7 + 6 = 13
constexpr std::size_t m_parameter = lookup_options_size + lookup_constraints_size;
constexpr std::size_t input_size_alphas = m_parameter - 1;

constexpr std::size_t input_size_lookup_gate_selectors = lookup_gate_size;
constexpr std::size_t input_size_lookup_gate_constraints_table_ids = lookup_constraints_size;
constexpr std::size_t input_size_lookup_gate_constraints_lookup_inputs = lookup_input_columns_size;

constexpr std::size_t input_size_lookup_table_selectors = lookup_table_size;
constexpr std::size_t input_size_lookup_table_lookup_options = lookup_value_columns_size;

constexpr std::size_t input_size_shifted_lookup_table_selectors = lookup_table_size;
constexpr std::size_t input_size_shifted_lookup_table_lookup_options = lookup_value_columns_size;

constexpr std::size_t input_size_sorted = m_parameter * 3 - 1;

typedef __attribute__((ext_vector_type(2))) typename pallas::base_field_type::value_type pair_type;

typedef __attribute__((ext_vector_type(4))) typename pallas::base_field_type::value_type output_type;

[[circuit]] output_type gate_argument_verifier(
    std::array<typename pallas::base_field_type::value_type, input_size_alphas>
        alphas,
    std::array<typename pallas::base_field_type::value_type, input_size_lookup_gate_selectors>
        lookup_gate_selectors,
    std::array<typename pallas::base_field_type::value_type, input_size_lookup_gate_constraints_table_ids>
        lookup_gate_constraints_table_ids,
    std::array<typename pallas::base_field_type::value_type, input_size_lookup_gate_constraints_lookup_inputs>
        lookup_gate_constraints_lookup_inputs,
    std::array<typename pallas::base_field_type::value_type, input_size_lookup_table_selectors>
        lookup_table_selectors,
    std::array<typename pallas::base_field_type::value_type, input_size_lookup_table_lookup_options>
        lookup_table_lookup_options,
    std::array<typename pallas::base_field_type::value_type, input_size_shifted_lookup_table_selectors>
        shifted_lookup_table_selectors,
    std::array<typename pallas::base_field_type::value_type, input_size_shifted_lookup_table_lookup_options>
        shifted_lookup_table_lookup_options,
    std::array<typename pallas::base_field_type::value_type, input_size_sorted>
        sorted,
    typename pallas::base_field_type::value_type theta,
    typename pallas::base_field_type::value_type beta,
    typename pallas::base_field_type::value_type gamma,
    typename pallas::base_field_type::value_type L0,
    pair_type V_L_values,
    pair_type q_last,
    pair_type q_blind

) {

    return __builtin_assigner_lookup_arg_verifier(
        (int*)&lookup_table_lookup_options_sizes, lookup_table_size, (int*)&lookup_table_columns_numbers,
        lookup_table_size, (int*)&lookup_gate_constraints_sizes, lookup_gate_size,
        (int*)&lookup_gate_constraints_lookup_input_sizes,
        lookup_constraints_size,    // sum of the lookup_gate_constraints_sizes elements

        alphas.data(), input_size_alphas, lookup_gate_selectors.data(), input_size_lookup_gate_selectors,
        lookup_gate_constraints_table_ids.data(), input_size_lookup_gate_constraints_table_ids,
        lookup_gate_constraints_lookup_inputs.data(), input_size_lookup_gate_constraints_lookup_inputs,
        lookup_table_selectors.data(), input_size_lookup_table_selectors, lookup_table_lookup_options.data(),
        input_size_lookup_table_lookup_options, shifted_lookup_table_selectors.data(),
        input_size_shifted_lookup_table_selectors, shifted_lookup_table_lookup_options.data(),
        input_size_shifted_lookup_table_lookup_options, sorted.data(), input_size_sorted, theta, beta, gamma, L0,
        V_L_values, q_last, q_blind);
}
