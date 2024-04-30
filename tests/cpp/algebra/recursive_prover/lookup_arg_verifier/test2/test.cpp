#ifndef __ZKLLVM__
#include "../../../../read_boost_json.hpp"
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

#define ARRAY_SUM_3(arr) arr[0] + arr[1] + arr[2]
#define ARRAY_SCALAR_MUL_3(arr1, arr2) arr1[0] * arr2[0] + arr1[1] * arr2[1] + arr1[2] * arr2[2]

constexpr std::size_t lookup_gate_size = 3;
constexpr std::size_t lookup_table_size = 3;

constexpr std::array<std::size_t, lookup_table_size> lookup_table_lookup_options_sizes = {1, 2, 3};
constexpr std::array<std::size_t, lookup_table_size> lookup_table_columns_numbers = {7, 2, 1};

constexpr std::size_t lookup_options_size = ARRAY_SUM_3(lookup_table_lookup_options_sizes);
constexpr std::size_t lookup_value_columns_size =
    ARRAY_SCALAR_MUL_3(lookup_table_lookup_options_sizes, lookup_table_columns_numbers);

constexpr std::array<std::size_t, lookup_gate_size> lookup_gate_constraints_sizes = {1, 1, 1};

constexpr std::size_t lookup_constraints_size = ARRAY_SUM_3(lookup_gate_constraints_sizes);
constexpr std::array<std::size_t, lookup_constraints_size> lookup_gate_constraints_lookup_input_sizes = {7, 2, 1};

constexpr std::size_t lookup_input_columns_size = ARRAY_SUM_3(lookup_gate_constraints_lookup_input_sizes);

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

#ifdef __ZKLLVM__
typedef __attribute__((ext_vector_type(2))) typename pallas::base_field_type::value_type pair_type;

typedef __attribute__((ext_vector_type(4))) typename pallas::base_field_type::value_type output_type;
#else
typedef std::array<typename pallas::base_field_type::value_type, 2> pair_type;
typedef std::array<typename pallas::base_field_type::value_type, 4> output_type;
#endif

[[circuit]] output_type lookup_argument_verifier(
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
    pair_type q_blind,
    output_type expected_res) {

#ifdef __ZKLLVM__
    return __builtin_assigner_lookup_arg_verifier(
        (int *)&lookup_table_lookup_options_sizes, lookup_table_size, (int *)&lookup_table_columns_numbers,
        lookup_table_size, (int *)&lookup_gate_constraints_sizes, lookup_gate_size,
        (int *)&lookup_gate_constraints_lookup_input_sizes,
        lookup_constraints_size,    // sum of the lookup_gate_constraints_sizes elements

        alphas.data(), input_size_alphas, lookup_gate_selectors.data(), input_size_lookup_gate_selectors,
        lookup_gate_constraints_table_ids.data(), input_size_lookup_gate_constraints_table_ids,
        lookup_gate_constraints_lookup_inputs.data(), input_size_lookup_gate_constraints_lookup_inputs,
        lookup_table_selectors.data(), input_size_lookup_table_selectors, lookup_table_lookup_options.data(),
        input_size_lookup_table_lookup_options, shifted_lookup_table_selectors.data(),
        input_size_shifted_lookup_table_selectors, shifted_lookup_table_lookup_options.data(),
        input_size_shifted_lookup_table_lookup_options, sorted.data(), input_size_sorted, theta, beta, gamma, L0,
        V_L_values, q_last, q_blind);
#else
    for (std::size_t i = 0; i < expected_res.size(); i++) {
        std::cout << expected_res[i].data << "\n";
    }
    return expected_res;
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

    std::size_t i = 0;

    std::array<typename pallas::base_field_type::value_type, input_size_alphas> alphas =
        read_array_field<BlueprintFieldType, input_size_alphas>(input_json, i++);
    std::array<typename pallas::base_field_type::value_type, input_size_lookup_gate_selectors> lookup_gate_selectors =
        read_array_field<BlueprintFieldType, input_size_lookup_gate_selectors>(input_json, i++);
    std::array<typename pallas::base_field_type::value_type, input_size_lookup_gate_constraints_table_ids>
        lookup_gate_constraints_table_ids =
            read_array_field<BlueprintFieldType, input_size_lookup_gate_constraints_table_ids>(input_json, i++);
    std::array<typename pallas::base_field_type::value_type, input_size_lookup_gate_constraints_lookup_inputs>
        lookup_gate_constraints_lookup_inputs =
            read_array_field<BlueprintFieldType, input_size_lookup_gate_constraints_lookup_inputs>(input_json, i++);
    std::array<typename pallas::base_field_type::value_type, input_size_lookup_table_selectors> lookup_table_selectors =
        read_array_field<BlueprintFieldType, input_size_lookup_table_selectors>(input_json, i++);
    std::array<typename pallas::base_field_type::value_type, input_size_lookup_table_lookup_options>
        lookup_table_lookup_options =
            read_array_field<BlueprintFieldType, input_size_lookup_table_lookup_options>(input_json, i++);
    std::array<typename pallas::base_field_type::value_type, input_size_shifted_lookup_table_selectors>
        shifted_lookup_table_selectors =
            read_array_field<BlueprintFieldType, input_size_shifted_lookup_table_selectors>(input_json, i++);
    std::array<typename pallas::base_field_type::value_type, input_size_shifted_lookup_table_lookup_options>
        shifted_lookup_table_lookup_options =
            read_array_field<BlueprintFieldType, input_size_shifted_lookup_table_lookup_options>(input_json, i++);
    std::array<typename pallas::base_field_type::value_type, input_size_sorted> sorted =
        read_array_field<BlueprintFieldType, input_size_sorted>(input_json, i++);
    typename pallas::base_field_type::value_type theta = read_field<BlueprintFieldType>(input_json, i++);
    typename pallas::base_field_type::value_type beta = read_field<BlueprintFieldType>(input_json, i++);
    typename pallas::base_field_type::value_type gamma = read_field<BlueprintFieldType>(input_json, i++);
    typename pallas::base_field_type::value_type L0 = read_field<BlueprintFieldType>(input_json, i++);
    pair_type V_L_values = read_vector_field<BlueprintFieldType, 2>(input_json, i++);
    pair_type q_last = read_vector_field<BlueprintFieldType, 2>(input_json, i++);
    pair_type q_blind = read_vector_field<BlueprintFieldType, 2>(input_json, i++);
    output_type expected_res = read_vector_field<BlueprintFieldType, 4>(input_json, i++);

    lookup_argument_verifier(alphas,
                             lookup_gate_selectors,
                             lookup_gate_constraints_table_ids,
                             lookup_gate_constraints_lookup_inputs,
                             lookup_table_selectors,
                             lookup_table_lookup_options,
                             shifted_lookup_table_selectors,
                             shifted_lookup_table_lookup_options,
                             sorted,
                             theta,
                             beta,
                             gamma,
                             L0,
                             V_L_values,
                             q_last,
                             q_blind,
                             expected_res);

    return 0;
}
#endif
