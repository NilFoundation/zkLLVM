//---------------------------------------------------------------------------//
// Copyright (c) 2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2021 Nikita Kaskov <nbering@nil.foundation>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//---------------------------------------------------------------------------//

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>

#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#define BOOST_FILESYSTEM_NO_DEPRECATED
#endif
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#define BOOST_SYSTEM_NO_DEPRECATED
#endif

#include <boost/json/src.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>

#include <nil/crypto3/algebra/curves/pallas.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/pallas.hpp>
#include <nil/crypto3/algebra/curves/ed25519.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/ed25519.hpp>
#include <nil/crypto3/algebra/curves/bls12.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/bls12.hpp>

#include <nil/crypto3/zk/snark/arithmetization/plonk/params.hpp>
#include <nil/crypto3/zk/snark/arithmetization/plonk/constraint_system.hpp>

#include <ios>

#include <nil/crypto3/hash/sha2.hpp>
#include <nil/crypto3/hash/poseidon.hpp>

#include <nil/marshalling/status_type.hpp>
#include <nil/marshalling/field_type.hpp>
#include <nil/marshalling/endianness.hpp>
#include <nil/crypto3/marshalling/zk/types/plonk/constraint_system.hpp>
#include <nil/crypto3/marshalling/zk/types/plonk/assignment_table.hpp>

#include <nil/blueprint/assigner.hpp>
#include <nil/blueprint/asserts.hpp>
#include <nil/blueprint/utils/satisfiability_check.hpp>

#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Signals.h>

using namespace nil;
using namespace nil::crypto3;
using namespace nil::blueprint;

template<typename Endianness, typename ArithmetizationType, typename ConstraintSystemType>
void print_circuit(const circuit_proxy<ArithmetizationType> &circuit_proxy,
                   const assignment_proxy<ArithmetizationType> &table_proxy,
                   bool rename_required, std::ostream &out = std::cout) {
    using TTypeBase = nil::marshalling::field_type<Endianness>;
    using plonk_constraint_system = nil::marshalling::types::bundle<
        TTypeBase, std::tuple<
                       nil::crypto3::marshalling::types::plonk_gates<TTypeBase, typename ConstraintSystemType::gates_container_type::value_type >,               // gates
                       nil::crypto3::marshalling::types::plonk_copy_constraints<TTypeBase, typename ConstraintSystemType::field_type>,                           // copy constraints
                       nil::crypto3::marshalling::types::plonk_lookup_gates<TTypeBase, typename ConstraintSystemType::lookup_gates_container_type::value_type>,  // lookup constraints
                       // If we don't have lookup gates, we don't need lookup tables
                       nil::crypto3::marshalling::types::plonk_lookup_tables< TTypeBase, typename ConstraintSystemType::lookup_tables_type::value_type >         // lookup tables
                       >
        >;
    using AssignmentTableType = assignment_proxy<ArithmetizationType>;
    using variable_type = crypto3::zk::snark::plonk_variable<typename AssignmentTableType::field_type::value_type>;

    const auto& gates = circuit_proxy.gates();
    const auto& used_gates_idx = circuit_proxy.get_used_gates();
    typename ConstraintSystemType::gates_container_type used_gates;
    for (const auto &it : used_gates_idx) {
        used_gates.push_back(gates[it]);
    }

    const auto& copy_constraints = circuit_proxy.copy_constraints();
    typename ConstraintSystemType::copy_constraints_container_type used_copy_constraints;
    const auto& used_copy_constraints_idx = circuit_proxy.get_used_copy_constraints();
    for (const auto &it : used_copy_constraints_idx) {
        used_copy_constraints.push_back(copy_constraints[it]);
    }

    if (rename_required) {
        const auto& used_rows = table_proxy.get_used_rows();
        std::uint32_t local_row = 0;
        for (const auto &row : used_rows) {
            for (auto &constraint : used_copy_constraints) {
                const auto first_var = constraint.first;
                const auto second_var = constraint.second;
                if ((first_var.type == variable_type::column_type::witness ||
                     first_var.type == variable_type::column_type::constant) &&
                    first_var.rotation == row) {
                    constraint.first = variable_type(first_var.index, local_row, first_var.relative,
                                                     first_var.type);
                }
                if ((second_var.type == variable_type::column_type::witness ||
                     second_var.type == variable_type::column_type::constant) &&
                    second_var.rotation == row) {
                    constraint.second = variable_type(second_var.index, local_row,
                                                      second_var.relative, second_var.type);
                }
            }
            local_row++;
        }
    }

    const auto& lookup_gates = circuit_proxy.lookup_gates();
    typename ConstraintSystemType::lookup_gates_container_type used_lookup_gates;
    const auto& used_lookup_gates_idx = circuit_proxy.get_used_lookup_gates();
    for (const auto &it : used_lookup_gates_idx) {
        used_lookup_gates.push_back(lookup_gates[it]);
    }

    const auto& lookup_tables = circuit_proxy.lookup_tables();
    typename ConstraintSystemType::lookup_tables_type used_lookup_tables;
    const auto& used_lookup_tables_idx = circuit_proxy.get_used_lookup_tables();
    for (const auto &it : used_lookup_tables_idx) {
        used_lookup_tables.push_back(lookup_tables[it]);
    }

    auto filled_val =
        plonk_constraint_system(std::make_tuple(
            nil::crypto3::marshalling::types::fill_plonk_gates<Endianness, typename ConstraintSystemType::gates_container_type::value_type>(used_gates),
            nil::crypto3::marshalling::types::fill_plonk_copy_constraints<Endianness, typename ConstraintSystemType::variable_type>(used_copy_constraints),
            nil::crypto3::marshalling::types::fill_plonk_lookup_gates<Endianness, typename ConstraintSystemType::lookup_gates_container_type::value_type>(used_lookup_gates),
            nil::crypto3::marshalling::types::fill_plonk_lookup_tables<Endianness, typename ConstraintSystemType::lookup_tables_type::value_type>(used_lookup_tables)
                ));

    std::vector<std::uint8_t> cv;
    cv.resize(filled_val.length(), 0x00);
    auto write_iter = cv.begin();
    nil::marshalling::status_type status = filled_val.write(write_iter, cv.size());
    out.write(reinterpret_cast<char*>(cv.data()), cv.size());
}

enum class print_table_kind {
    MULTI_PROVER,
    SINGLE_PROVER
};

enum class print_column_kind {
    WITNESS,
    SHARED,
    PUBLIC_INPUT,
    CONSTANT,
    SELECTOR
};

template<typename ValueType, typename ContainerType>
void fill_vector_value(std::vector<ValueType> &table_values, const ContainerType &table_col, typename std::vector<ValueType>::iterator start) {
    std::copy(table_col.begin(), table_col.end(), start);
}

template<typename Endianness, typename ArithmetizationType, typename BlueprintFieldType>
void print_assignment_table(const assignment_proxy<ArithmetizationType> &table_proxy,
                            print_table_kind print_kind,
                            std::uint32_t ComponentConstantColumns, std::uint32_t ComponentSelectorColumns,
                            std::ostream &out = std::cout) {
    using AssignmentTableType = assignment_proxy<ArithmetizationType>;
    std::uint32_t usable_rows_amount;
    std::uint32_t total_columns;
    std::uint32_t total_size;
    std::uint32_t shared_size = (print_kind == print_table_kind::MULTI_PROVER) ? 1 : 0;
    std::uint32_t public_input_size = table_proxy.public_inputs_amount();
    std::uint32_t witness_size = table_proxy.witnesses_amount();
    std::uint32_t constant_size = table_proxy.constants_amount();
    std::uint32_t selector_size = table_proxy.selectors_amount();
    const auto lookup_constant_cols = table_proxy.get_lookup_constant_cols();
    const auto lookup_selector_cols = table_proxy.get_lookup_selector_cols();

    std::uint32_t max_public_inputs_size = 0;
    std::uint32_t max_constant_size = 0;
    std::uint32_t max_selector_size = 0;

    for (std::uint32_t i = 0; i < public_input_size; i++) {
        max_public_inputs_size = std::max(max_public_inputs_size, table_proxy.public_input_column_size(i));
    }

    if (print_kind == print_table_kind::MULTI_PROVER) {
        total_columns = witness_size + shared_size + public_input_size + constant_size + selector_size;
        std::uint32_t max_shared_size = 0;
        for (std::uint32_t i = 0; i < shared_size; i++) {
            max_shared_size = std::max(max_shared_size, table_proxy.shared_column_size(i));
        }
        for (const auto &i : lookup_constant_cols) {
            max_constant_size = std::max(max_constant_size, table_proxy.constant_column_size(i));
        }
        for (const auto &i : lookup_selector_cols) {
            max_selector_size = std::max(max_selector_size, table_proxy.selector_column_size(i));
        }
        usable_rows_amount = table_proxy.get_used_rows().size();
        usable_rows_amount = std::max({usable_rows_amount, max_shared_size, max_public_inputs_size, max_constant_size, max_selector_size});
    } else { // SINGLE_PROVER
        total_columns = witness_size + shared_size + public_input_size + constant_size + selector_size;
        std::uint32_t max_witness_size = 0;
        for (std::uint32_t i = 0; i < witness_size; i++) {
            max_witness_size = std::max(max_witness_size, table_proxy.witness_column_size(i));
        }
        for (std::uint32_t i = 0; i < constant_size; i++) {
            max_constant_size = std::max(max_constant_size, table_proxy.constant_column_size(i));
        }
        for (std::uint32_t i = 0; i < selector_size; i++) {
            max_selector_size = std::max(max_selector_size, table_proxy.selector_column_size(i));
        }
        usable_rows_amount = std::max({max_witness_size, max_public_inputs_size, max_constant_size, max_selector_size});
    }

    std::uint32_t padded_rows_amount = std::pow(2, std::ceil(std::log2(usable_rows_amount)));
    if (padded_rows_amount == usable_rows_amount) {
        padded_rows_amount *= 2;
    }
    if (padded_rows_amount < 8) {
        padded_rows_amount = 8;
    }
    total_size = padded_rows_amount * total_columns;

    using TTypeBase = nil::marshalling::field_type<Endianness>;
    using plonk_assignment_table = nil::marshalling::types::bundle<
        TTypeBase,
        std::tuple<
            nil::marshalling::types::integral<TTypeBase, std::size_t>, // witness_amount
            nil::marshalling::types::integral<TTypeBase, std::size_t>, // public_input_amount
            nil::marshalling::types::integral<TTypeBase, std::size_t>, // constant_amount
            nil::marshalling::types::integral<TTypeBase, std::size_t>, // selector_amount

            nil::marshalling::types::integral<TTypeBase, std::size_t>, // usable_rows
            nil::marshalling::types::integral<TTypeBase, std::size_t>, // padded_rows_amount

            // table_witness_values
            nil::marshalling::types::array_list<
                TTypeBase,
                nil::crypto3::marshalling::types::field_element<TTypeBase, typename AssignmentTableType::field_type::value_type>,
                nil::marshalling::option::sequence_size_field_prefix<nil::marshalling::types::integral<TTypeBase, std::size_t>>
            >,
            // table_public_input_values
            nil::marshalling::types::array_list<
                TTypeBase,
                nil::crypto3::marshalling::types::field_element<TTypeBase, typename AssignmentTableType::field_type::value_type>,
                nil::marshalling::option::sequence_size_field_prefix<nil::marshalling::types::integral<TTypeBase, std::size_t>>
            >,
            // table_constant_values
            nil::marshalling::types::array_list<
                TTypeBase,
                nil::crypto3::marshalling::types::field_element<TTypeBase, typename AssignmentTableType::field_type::value_type>,
                nil::marshalling::option::sequence_size_field_prefix<nil::marshalling::types::integral<TTypeBase, std::size_t>>
            >,
            // table_selector_values
            nil::marshalling::types::array_list<
                TTypeBase,
                nil::crypto3::marshalling::types::field_element<TTypeBase, typename AssignmentTableType::field_type::value_type>,
                nil::marshalling::option::sequence_size_field_prefix<nil::marshalling::types::integral<TTypeBase, std::size_t>>
            >
        >
    >;
    using column_type = typename crypto3::zk::snark::plonk_column<BlueprintFieldType>;

    std::vector<typename AssignmentTableType::field_type::value_type> table_witness_values(     padded_rows_amount * witness_size ,     0);
    std::vector<typename AssignmentTableType::field_type::value_type> table_public_input_values(padded_rows_amount * (public_input_size + shared_size), 0);
    std::vector<typename AssignmentTableType::field_type::value_type> table_constant_values(    padded_rows_amount * constant_size,     0);
    std::vector<typename AssignmentTableType::field_type::value_type> table_selector_values(    padded_rows_amount * selector_size,     0);
    if (print_kind == print_table_kind::SINGLE_PROVER) {
        auto it = table_witness_values.begin();
        for (std::uint32_t i = 0; i < witness_size; i++) {
            fill_vector_value<typename AssignmentTableType::field_type::value_type, column_type>
                (table_witness_values, table_proxy.witness(i), it);
            it += padded_rows_amount;
        }
        it = table_public_input_values.begin();
        for (std::uint32_t i = 0; i < public_input_size; i++) {
            fill_vector_value<typename AssignmentTableType::field_type::value_type, column_type>
                (table_public_input_values, table_proxy.public_input(i), it);
            it += padded_rows_amount;
        }
        it = table_constant_values.begin();
        for (std::uint32_t i = 0; i < constant_size; i++) {
            fill_vector_value<typename AssignmentTableType::field_type::value_type, column_type>
                (table_constant_values, table_proxy.constant(i), it);
            it += padded_rows_amount;
        }
        it = table_selector_values.begin();
        for (std::uint32_t i = 0; i < selector_size; i++) {
            fill_vector_value<typename AssignmentTableType::field_type::value_type, column_type>
                (table_selector_values, table_proxy.selector(i), it);
            it += padded_rows_amount;
        }
    } else {
        const auto& rows = table_proxy.get_used_rows();
        const auto& selector_rows = table_proxy.get_used_selector_rows();
        std::uint32_t witness_idx = 0;
        // witness
        for( std::size_t i = 0; i < witness_size; i++ ){
            const auto column_size = table_proxy.witness_column_size(i);
            std::uint32_t offset = 0;
            for(const auto& j : rows){
                if (j < column_size) {
                    table_witness_values[witness_idx + offset] = table_proxy.witness(i, j);
                    offset++;
                }
            }
            witness_idx += padded_rows_amount;
        }
        // public input
        std::uint32_t pub_inp_idx = 0;
        auto it_pub_inp = table_public_input_values.begin();
        for (std::uint32_t i = 0; i < public_input_size; i++) {
            fill_vector_value<typename AssignmentTableType::field_type::value_type, column_type>
                (table_public_input_values, table_proxy.public_input(i), it_pub_inp);
            it_pub_inp += padded_rows_amount;
            pub_inp_idx += padded_rows_amount;
        }
        for (std::uint32_t i = 0; i < shared_size; i++) {
            fill_vector_value<typename AssignmentTableType::field_type::value_type, column_type>
                (table_public_input_values, table_proxy.shared(i), it_pub_inp);
            it_pub_inp += padded_rows_amount;
            pub_inp_idx += padded_rows_amount;
        }
        // constant
        std::uint32_t constant_idx = 0;
        for (std::uint32_t i = 0; i < ComponentConstantColumns; i++) {
            const auto column_size = table_proxy.constant_column_size(i);
            std::uint32_t offset = 0;
            for(const auto& j : rows){
                if (j < column_size) {
                    table_constant_values[constant_idx + offset] = table_proxy.constant(i, j);
                    offset++;
                }
            }

            constant_idx += padded_rows_amount;
        }

        auto it_const = table_constant_values.begin() + constant_idx;
        for (std::uint32_t i = ComponentConstantColumns; i < constant_size; i++) {
            fill_vector_value<typename AssignmentTableType::field_type::value_type, column_type>
                (table_constant_values, table_proxy.constant(i), it_const);
            it_const += padded_rows_amount;
            constant_idx += padded_rows_amount;
        }
        // selector
        std::uint32_t selector_idx = 0;
        for (std::uint32_t i = 0; i < ComponentSelectorColumns; i++) {
            const auto column_size = table_proxy.selector_column_size(i);
            std::uint32_t offset = 0;
            for(const auto& j : rows){
                if (j < column_size) {
                    if (selector_rows.find(j) != selector_rows.end()) {

                        table_selector_values[selector_idx + offset] = table_proxy.selector(i, j);
                    }
                    offset++;
                }
            }
            selector_idx += padded_rows_amount;
        }

        auto it_selector = table_selector_values.begin();
        for (std::uint32_t i = ComponentSelectorColumns; i < selector_size; i++) {
            fill_vector_value<typename AssignmentTableType::field_type::value_type, column_type>
                (table_selector_values, table_proxy.selector(i), it_selector);
            it_selector += padded_rows_amount;
            selector_idx += padded_rows_amount;
        }
        ASSERT_MSG(witness_idx + pub_inp_idx + constant_idx + selector_idx == total_size, "Printed index not equal required assignment size" );
    }

    auto filled_val = plonk_assignment_table(std::make_tuple(
        nil::marshalling::types::integral<TTypeBase, std::size_t>(witness_size),
        nil::marshalling::types::integral<TTypeBase, std::size_t>(public_input_size + shared_size),
        nil::marshalling::types::integral<TTypeBase, std::size_t>(constant_size),
        nil::marshalling::types::integral<TTypeBase, std::size_t>(selector_size),
        nil::marshalling::types::integral<TTypeBase, std::size_t>(usable_rows_amount),
        nil::marshalling::types::integral<TTypeBase, std::size_t>(padded_rows_amount),
        nil::crypto3::marshalling::types::fill_field_element_vector<typename AssignmentTableType::field_type::value_type, Endianness>(table_witness_values),
        nil::crypto3::marshalling::types::fill_field_element_vector<typename AssignmentTableType::field_type::value_type, Endianness>(table_public_input_values),
        nil::crypto3::marshalling::types::fill_field_element_vector<typename AssignmentTableType::field_type::value_type, Endianness>(table_constant_values),
        nil::crypto3::marshalling::types::fill_field_element_vector<typename AssignmentTableType::field_type::value_type, Endianness>(table_selector_values)
    ));

    table_witness_values.clear();
    table_witness_values.shrink_to_fit();

    table_public_input_values.clear();
    table_public_input_values.shrink_to_fit();

    table_constant_values.clear();
    table_constant_values.shrink_to_fit();

    table_selector_values.clear();
    table_selector_values.shrink_to_fit();

    std::vector<std::uint8_t> cv;
    cv.resize(filled_val.length(), 0x00);
    auto write_iter = cv.begin();
    nil::marshalling::status_type status = filled_val.write(write_iter, cv.size());
    out.write(reinterpret_cast<char*>(cv.data()), cv.size());
}

bool read_json(
    std::string input_file_name,
    boost::json::value &input_json_value
) {
    std::ifstream input_file(input_file_name.c_str());
    if (!input_file.is_open()) {
        std::cerr << "Could not open the file - '" << input_file_name << "'" << std::endl;
        return false;
    }

    boost::json::stream_parser p;
    boost::json::error_code ec;
    while (!input_file.eof()) {
        char input_string[256];
        input_file.read(input_string, sizeof(input_string) - 1);
        input_string[input_file.gcount()] = '\0';
        p.write(input_string, ec);
        if (ec) {
            std::cerr << "JSON parsing of input failed" << std::endl;
            return false;
        }
    }
    p.finish(ec);
    if (ec) {
        std::cerr << "JSON parsing of input failed" << std::endl;
        return false;
    }
    input_json_value = p.release();
    if (!input_json_value.is_array()) {
        std::cerr << "Array of arguments is expected in JSON file" << std::endl;
        return false;
    }
    return true;
}

struct ParametersPolicy {
    constexpr static const std::size_t WitnessColumns = WITNESS_COLUMNS;
    constexpr static const std::size_t PublicInputColumns = PUBLIC_INPUT_COLUMNS;
    constexpr static const std::size_t ComponentConstantColumns = COMPONENT_CONSTANT_COLUMNS;
    constexpr static const std::size_t LookupConstantColumns = LOOKUP_CONSTANT_COLUMNS;
    constexpr static const std::size_t ComponentSelectorColumns = COMPONENT_SELECTOR_COLUMNS;
    constexpr static const std::size_t LookupSelectorColumns = LOOKUP_SELECTOR_COLUMNS;
};

template<typename BlueprintFieldType>
int curve_dependent_main(std::string bytecode_file_name,
                          std::string public_input_file_name,
                          std::string private_input_file_name,
                          std::string assignment_table_file_name,
                          std::string circuit_file_name,
                          std::string processed_public_input_file_name,
                          long stack_size,
                          bool check_validity,
                          boost::log::trivial::severity_level log_level,
                          const std::string &policy,
                          nil::blueprint::generation_mode gen_mode,
                          std::uint32_t max_num_provers,
                          std::uint32_t max_lookup_rows,
                          std::uint32_t target_prover,
                          nil::blueprint::print_format circuit_output_print_format) {

    constexpr std::size_t ComponentConstantColumns = ParametersPolicy::ComponentConstantColumns;
    constexpr std::size_t LookupConstantColumns = ParametersPolicy::LookupConstantColumns;
    constexpr std::size_t ComponentSelectorColumns = ParametersPolicy::ComponentSelectorColumns;
    constexpr std::size_t LookupSelectorColumns = ParametersPolicy::LookupSelectorColumns;

    constexpr std::size_t WitnessColumns = ParametersPolicy::WitnessColumns;
    constexpr std::size_t PublicInputColumns = ParametersPolicy::PublicInputColumns;
    constexpr std::size_t ConstantColumns = ComponentConstantColumns + LookupConstantColumns;
    constexpr std::size_t SelectorColumns = ComponentSelectorColumns + LookupSelectorColumns;

    zk::snark::plonk_table_description<BlueprintFieldType> desc(
        WitnessColumns, PublicInputColumns, ConstantColumns, SelectorColumns);
    using ConstraintSystemType = zk::snark::plonk_constraint_system<BlueprintFieldType>;
    using ConstraintSystemProxyType = zk::snark::plonk_table<BlueprintFieldType, zk::snark::plonk_column<BlueprintFieldType>>;
    using ArithmetizationType =
            crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType>;
    using AssignmentTableType = zk::snark::plonk_table<BlueprintFieldType, zk::snark::plonk_column<BlueprintFieldType>>;

    boost::json::value public_input_json_value;
    if(public_input_file_name.empty()) {
        public_input_json_value = boost::json::array();
    } else if (!read_json(public_input_file_name, public_input_json_value)){
        return 1;
    }

    boost::json::value private_input_json_value;
    if(private_input_file_name.empty()) {
        private_input_json_value = boost::json::array();
    } else if (!read_json(private_input_file_name, private_input_json_value)) {
        return 1;
    }

    nil::blueprint::assigner<BlueprintFieldType> assigner_instance(
        desc,
        stack_size,
        log_level,
        max_num_provers,
        target_prover,
        gen_mode,
        policy,
        circuit_output_print_format,
        check_validity
    );

    if (!assigner_instance.parse_ir_file(bytecode_file_name.c_str())) {
        return 1;
    }
    if (!processed_public_input_file_name.empty()) {
        if (!assigner_instance.dump_public_input(public_input_json_value.as_array(), processed_public_input_file_name)) {
            return 1;
        }
        return 0;
    }

    if (!assigner_instance.evaluate(public_input_json_value.as_array(), private_input_json_value.as_array())) {
        return 1;
    }

    ASSERT_MSG(!assigner_instance.assignments.empty() && !assigner_instance.circuits.empty(), "Not found any proxy for prover" );

    if (gen_mode.has_size_estimation()) {
        return 0;
    }

    // pack lookup tables
    if (assigner_instance.circuits[0].get_reserved_tables().size() > 0) {
        std::vector <std::size_t> lookup_columns_indices;
        lookup_columns_indices.resize(LookupConstantColumns);
        // fill ComponentConstantColumns, ComponentConstantColumns + 1, ...
        std::iota(lookup_columns_indices.begin(), lookup_columns_indices.end(), ComponentConstantColumns);

        auto usable_rows_amount = zk::snark::pack_lookup_tables_horizontal(
                assigner_instance.circuits[0].get_reserved_indices(),
                assigner_instance.circuits[0].get_reserved_tables(),
                assigner_instance.circuits[0].get(),
                assigner_instance.assignments[0].get(),
                lookup_columns_indices,
                ComponentSelectorColumns,
                0,
                max_lookup_rows
        );
    }

    constexpr std::uint32_t invalid_target_prover = std::numeric_limits<std::uint32_t>::max();
    // print assignment tables and circuits
    ASSERT_MSG(assigner_instance.assignments.size() == assigner_instance.circuits.size(), "Missmatch assignments circuits size");
    if (assigner_instance.assignments.size() == 1 && (target_prover == 0 || target_prover == invalid_target_prover)) {
        // print assignment table
        if (gen_mode.has_assignments()) {
            std::ofstream otable;
            otable.open(assignment_table_file_name, std::ios_base::binary | std::ios_base::out);
            if (!otable) {
                std::cout << "Something wrong with output " << assignment_table_file_name << std::endl;
                return 1;
            }

            print_assignment_table<nil::marshalling::option::big_endian, ArithmetizationType, BlueprintFieldType>(
                assigner_instance.assignments[0], print_table_kind::SINGLE_PROVER, ComponentConstantColumns,
                ComponentSelectorColumns, otable);

            otable.close();
        }

        // print circuit
        if (gen_mode.has_circuit()) {
            std::ofstream ocircuit;
            ocircuit.open(circuit_file_name, std::ios_base::binary | std::ios_base::out);
            if (!ocircuit) {
                std::cout << "Something wrong with output " << circuit_file_name << std::endl;
                return 1;
            }

            print_circuit<nil::marshalling::option::big_endian, ArithmetizationType, ConstraintSystemType>(
                assigner_instance.circuits[0], assigner_instance.assignments[0], false, ocircuit);
            ocircuit.close();
        }
    } else if (assigner_instance.assignments.size() > 1 &&
              (target_prover < assigner_instance.assignments.size() || target_prover == invalid_target_prover)) {
        std::uint32_t start_idx = (target_prover == invalid_target_prover) ? 0 : target_prover;
        std::uint32_t end_idx = (target_prover == invalid_target_prover) ? assigner_instance.assignments.size() : target_prover + 1;
        for (std::uint32_t idx = start_idx; idx < end_idx; idx++) {
            // print assignment table
            if (gen_mode.has_assignments()) {
                std::ofstream otable;
                otable.open(assignment_table_file_name + std::to_string(idx),
                            std::ios_base::binary | std::ios_base::out);
                if (!otable) {
                    std::cout << "Something wrong with output " << assignment_table_file_name + std::to_string(idx)
                              << std::endl;
                    return 1;
                }

                print_assignment_table<nil::marshalling::option::big_endian, ArithmetizationType, BlueprintFieldType>(
                    assigner_instance.assignments[idx], print_table_kind::MULTI_PROVER, ComponentConstantColumns,
                    ComponentSelectorColumns, otable);

                otable.close();
            }

            // print circuit
            if (gen_mode.has_circuit()) {
                std::ofstream ocircuit;
                ocircuit.open(circuit_file_name + std::to_string(idx), std::ios_base::binary | std::ios_base::out);
                if (!ocircuit) {
                    std::cout << "Something wrong with output " << circuit_file_name + std::to_string(idx) << std::endl;
                    return 1;
                }

                ASSERT_MSG(idx < assigner_instance.circuits.size(), "Not found circuit");
                print_circuit<nil::marshalling::option::big_endian, ArithmetizationType, ConstraintSystemType>(
                    assigner_instance.circuits[idx], assigner_instance.assignments[idx], (idx > 0), ocircuit);

                ocircuit.close();
            }
        }
    } else {
        std::cout << "No data for print: target prover " << target_prover << ", actual number of provers "
                  << assigner_instance.assignments.size() << std::endl;
        return 1;
    }

    if (check_validity && gen_mode.has_assignments() && gen_mode.has_circuit()) {
        if (assigner_instance.assignments.size() == 1 && (target_prover == 0 || target_prover == invalid_target_prover)) {
            ASSERT_MSG(nil::blueprint::is_satisfied(assigner_instance.circuits[0].get(), assigner_instance.assignments[0].get()),
                       "The circuit is not satisfied");
        } else if (assigner_instance.assignments.size() > 1 &&
                   (target_prover < assigner_instance.assignments.size() || target_prover == invalid_target_prover)) {
            //  check only for target prover if set
            std::uint32_t start_idx = (target_prover == invalid_target_prover) ? 0 : target_prover;
            std::uint32_t end_idx = (target_prover == invalid_target_prover) ? assigner_instance.assignments.size() : target_prover + 1;
            for (std::uint32_t idx = start_idx; idx < end_idx; idx++) {
                assigner_instance.assignments[idx].set_check(true);
                bool is_accessible =
                    nil::blueprint::is_satisfied(assigner_instance.circuits[idx], assigner_instance.assignments[idx]);
                assigner_instance.assignments[idx].set_check(false);
                ASSERT_MSG(is_accessible, ("The circuit is not satisfied on prover " + std::to_string(idx)).c_str());
            }
        } else {
            std::cout << "No data for check: target prover " << target_prover << ", actual number of provers "
                      << assigner_instance.assignments.size() << std::endl;
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {

    llvm::sys::PrintStackTraceOnErrorSignal(argv[0]);

    boost::program_options::options_description options_desc("zkLLVM assigner");

    // clang-format off
    options_desc.add_options()("help,h", "Display help message")
            ("version,v", "Display version")
            ("bytecode,b", boost::program_options::value<std::string>(), "Bytecode input file")
            ("public-input,i", boost::program_options::value<std::string>(), "Public input file")
            ("private-input,p", boost::program_options::value<std::string>(), "Private input file")
            ("assignment-table,t", boost::program_options::value<std::string>(), "Assignment table output file")
            ("circuit,c", boost::program_options::value<std::string>(), "Circuit output file")
            ("input-column", boost::program_options::value<std::string>(), "Output file for public input column")
            ("elliptic-curve-type,e", boost::program_options::value<std::string>(), "Native elliptic curve type (pallas, vesta, ed25519, bls12381)")
            ("stack-size,s", boost::program_options::value<long>(), "Stack size in bytes")
            ("check", "Check satisfiability of the generated circuit")
            ("log-level,l", boost::program_options::value<std::string>(), "Log level (trace, debug, info, warning, error, fatal)")
            ("print_circuit_output", "deprecated, use \"-f\" instead")
            ("print-circuit-output-format,f", boost::program_options::value<std::string>(), "print output of the circuit (dec, hex)")
            ("policy", boost::program_options::value<std::string>(), "Policy for creating circuits. Possible values: default")
            ("generate-type", boost::program_options::value<std::string>(), "Define generated output. Possible values: circuit, assignment, circuit-assignment, public-input-column, size_estimation(does not generate anything, just evaluates circuit size). Default value is circuit-assignment")
            ("max-num-provers", boost::program_options::value<int>(), "Maximum number of provers. Possible values >= 1")
            ("max-lookup-rows", boost::program_options::value<int>(), "Maximum number of provers. Possible values >= 1")
            ("target-prover", boost::program_options::value<int>(), "Assignment table and circuit will be generated only for defined prover. Possible values [0, max-num-provers)");
    // clang-format on


    boost::program_options::variables_map vm;
    try {
        boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(options_desc).run(),
                                    vm);
        boost::program_options::notify(vm);
    } catch (const boost::program_options::unknown_option &e) {
        std::cerr << "Invalid command line argument: " << e.what() << std::endl;
        std::cout << options_desc << std::endl;
        return 1;
    }


    if (vm.count("help")) {
        std::cout << options_desc << std::endl;
        return 0;
    }

    if (vm.count("version")) {
#ifdef ASSIGNER_VERSION
#define xstr(s) str(s)
#define str(s) #s
        std::cout << xstr(ASSIGNER_VERSION) << std::endl;
#else
        std::cout << "Version is not defined" << std::endl;
#endif
        return 0;
    }

    std::string bytecode_file_name;
    std::string public_input_file_name;
    std::string private_input_file_name;
    std::string assignment_table_file_name;
    std::string circuit_file_name;
    std::string processed_public_input_file_name;
    std::string elliptic_curve;
    nil::blueprint::print_format circuit_output_print_format;
    std::string log_level;
    long stack_size;

    if (vm.count("bytecode")) {
        bytecode_file_name = vm["bytecode"].as<std::string>();
    } else {
        std::cerr << "Invalid command line argument - bytecode file name is not specified" << std::endl;
        std::cout << options_desc << std::endl;
        return 1;
    }

    nil::blueprint::generation_mode gen_mode = nil::blueprint::generation_mode::assignments() | nil::blueprint::generation_mode::circuit();
    if (vm.count("generate-type")) {
        const auto generate_type = vm["generate-type"].as<std::string>();
        if (generate_type == "circuit") {
            gen_mode = nil::blueprint::generation_mode::circuit();
        } else if (generate_type == "assignment") {
            gen_mode = nil::blueprint::generation_mode::assignments();
        } else if (generate_type == "size_estimation") {
            gen_mode = nil::blueprint::generation_mode::size_estimation();
        } else if (generate_type == "public-input-column") {
            gen_mode = nil::blueprint::generation_mode::public_input_column();
        } else if (generate_type != "circuit-assignment") {
            std::cerr << "Invalid command line argument - generate-type. " << generate_type << " is wrong value." << std::endl;
            std::cout << options_desc << std::endl;
            return 1;
        }
    }

    if (!vm.count("public-input") && !vm.count("private-input") && gen_mode.has_assignments()) {
        std::cerr << "Both public and private input file names are not specified" << std::endl;
        std::cout << options_desc << std::endl;
        return 1;
    }

    if (gen_mode.has_public_input_column()) {
        if (vm.count("input-column")) {
            processed_public_input_file_name = vm["input-column"].as<std::string>();
        } else {
            std::cerr << "Expected \"--input-column\" argument with output file name" << std::endl;
            return 1;
        }
    }

    if (vm.count("private-input")) {
        private_input_file_name = vm["private-input"].as<std::string>();
    }

    if (vm.count("public-input")) {
        public_input_file_name = vm["public-input"].as<std::string>();
    }

    if (gen_mode.has_assignments()) {
        if (vm.count("assignment-table")) {
            assignment_table_file_name = vm["assignment-table"].as<std::string>();
        } else {
            std::cerr << "Invalid command line argument - assignment table file name is not specified" << std::endl;
            std::cout << options_desc << std::endl;
            return 1;
        }
    }

    if (gen_mode.has_circuit()) {
        if (vm.count("circuit")) {
            circuit_file_name = vm["circuit"].as<std::string>();
        } else {
            std::cerr << "Invalid command line argument - circuit file name is not specified" << std::endl;
            std::cout << options_desc << std::endl;
            return 1;
        }
    }

    if (vm.count("elliptic-curve-type")) {
        elliptic_curve = vm["elliptic-curve-type"].as<std::string>();
    } else {
        std::cerr << "Invalid command line argument - elliptic curve type is not specified" << std::endl;
        std::cout << options_desc << std::endl;
        return 1;
    }

    if (vm.count("log-level")) {
        log_level = vm["log-level"].as<std::string>();
    } else {
        log_level = "info";
    }


    std::map<std::string, int> curve_options{
        {"pallas", 0},
        {"vesta", 1},
        {"ed25519", 2},
        {"bls12381", 3},
    };

    if (curve_options.find(elliptic_curve) == curve_options.end()) {
        std::cerr << "Invalid command line argument -e (Native elliptic curve type): " << elliptic_curve << std::endl;
        std::cout << options_desc << std::endl;
        return 1;
    }


    std::map<std::string, nil::blueprint::print_format> print_circuit_output_options{
        {"dec", dec},
        {"hex", hex},
    };

    if (vm.count("print-circuit-output-format")) {
        std::string output_format = vm["print-circuit-output-format"].as<std::string>();
        if (print_circuit_output_options.find(output_format) == print_circuit_output_options.end()) {
            std::cerr << "Invalid command line argument -f (print-circuit-output-format): " << output_format << std::endl;
            std::cout << options_desc << std::endl;
            return 1;
        } else {
            circuit_output_print_format = print_circuit_output_options[output_format];
        }
    } else {
        circuit_output_print_format = no_print;
    }

    if (vm.count("stack-size")) {
        stack_size = vm["stack-size"].as<long>();
    } else {
        stack_size = 4000000;
    }

    std::string policy = "";
    if (vm.count("policy")) {
        policy = vm["policy"].as<std::string>();
        if (policy != "default") {
            std::cerr << "Invalid command line argument - policy. " << policy << " is wrong value." << std::endl;
            std::cout << options_desc << std::endl;
            return 1;
        }
    }

    std::uint32_t max_num_provers = 1;
    if (vm.count("max-num-provers")) {
        max_num_provers = vm["max-num-provers"].as<int>();
        if (max_num_provers < 1) {
            std::cerr << "Invalid command line argument - max-num-provers. " << max_num_provers << " is wrong value." << std::endl;
            std::cout << options_desc << std::endl;
            return 1;
        }
    }

    std::uint32_t max_lookup_rows = 500000;
    if (vm.count("max-lookup-rows")) {
        max_lookup_rows = vm["max-lookup-rows"].as<int>();
        if (max_lookup_rows < 1) {
            std::cerr << "Invalid command line argument - max-num-provers. " << max_num_provers << " is wrong value." << std::endl;
            std::cout << options_desc << std::endl;
            return 1;
        }
    }

    std::uint32_t target_prover = std::numeric_limits<std::uint32_t>::max();
    if (vm.count("target-prover")) {
        target_prover = vm["target-prover"].as<int>();
        if (target_prover >= max_num_provers) {
            std::cerr << "Invalid command line argument - target-prover. " << target_prover << " is wrong value." << std::endl;
            std::cout << options_desc << std::endl;
            return 1;
        }
    }

    // We use Boost log trivial severity levels, these are string representations of their names
    std::map<std::string, boost::log::trivial::severity_level> log_options{
        {"trace", boost::log::trivial::trace},
        {"debug", boost::log::trivial::debug},
        {"info", boost::log::trivial::info},
        {"warning", boost::log::trivial::warning},
        {"error", boost::log::trivial::error},
        {"fatal", boost::log::trivial::fatal}
    };

    if (log_options.find(log_level) == log_options.end()) {
        std::cerr << "Invalid command line argument -l (log level): " << log_level << std::endl;
        std::cout << options_desc << std::endl;
        return 1;
    }

    ASSERT_MSG(!vm.count("print_circuit_output"),
        "\nyou used --print_circuit_output flag,\n"
        "it is deprecated, use \"-f dec\" instead.\n"
        "Or use \"-f hex\", hex output format is also supported now\n"
    );

    switch (curve_options[elliptic_curve]) {
        case 0: {
            return curve_dependent_main<typename algebra::curves::pallas::base_field_type>(
                                                                          bytecode_file_name,
                                                                          public_input_file_name,
                                                                          private_input_file_name,
                                                                          assignment_table_file_name,
                                                                          circuit_file_name,
                                                                          processed_public_input_file_name,
                                                                          stack_size,
                                                                          vm.count("check"),
                                                                          log_options[log_level],
                                                                          policy,
                                                                          gen_mode,
                                                                          max_num_provers,
                                                                          max_lookup_rows,
                                                                          target_prover,
                                                                          circuit_output_print_format);
            break;
        }
        case 1: {
            UNREACHABLE("vesta curve based circuits are not supported yet");
            break;
        }
        case 2: {
            UNREACHABLE("ed25519 curve based circuits are not supported yet");
            break;
        }
        case 3: {
            return curve_dependent_main<typename algebra::fields::bls12_base_field<381>>(
                                                                          bytecode_file_name,
                                                                          public_input_file_name,
                                                                          private_input_file_name,
                                                                          assignment_table_file_name,
                                                                          circuit_file_name,
                                                                          processed_public_input_file_name,
                                                                          stack_size,
                                                                          vm.count("check"),
                                                                          log_options[log_level],
                                                                          policy,
                                                                          gen_mode,
                                                                          max_num_provers,
                                                                          max_lookup_rows,
                                                                          target_prover,
                                                                          circuit_output_print_format);
            break;
        }
    };

    return 0;
}
