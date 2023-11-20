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

#include <nil/crypto3/zk/snark/arithmetization/plonk/params.hpp>
#include <nil/crypto3/zk/snark/arithmetization/plonk/constraint_system.hpp>

#include <ios>

#include <nil/marshalling/status_type.hpp>
#include <nil/marshalling/field_type.hpp>
#include <nil/marshalling/endianness.hpp>
#include <nil/crypto3/marshalling/zk/types/plonk/constraint_system.hpp>
#include <nil/crypto3/marshalling/zk/types/plonk/assignment_table.hpp>

#include <nil/blueprint/parser.hpp>
#include <nil/blueprint/asserts.hpp>
#include <nil/blueprint/utils/satisfiability_check.hpp>

#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Signals.h>

using namespace nil;
using namespace nil::crypto3;
using namespace nil::blueprint;

template<typename TIter>
void print_hex_byteblob(std::ostream &os, TIter iter_begin, TIter iter_end, bool endl) {
    os << "0x" << std::hex;
    for (TIter it = iter_begin; it != iter_end; it++) {
        os << std::setfill('0') << std::setw(2) << std::right << int(*it);
    }
    os << std::dec;
    if (endl) {
        os << std::endl;
    }
}

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
                nil::crypto3::marshalling::types::fill_plonk_copy_constraints<Endianness, typename ConstraintSystemType::field_type>(used_copy_constraints),
                nil::crypto3::marshalling::types::fill_plonk_lookup_gates<Endianness, typename ConstraintSystemType::lookup_gates_container_type::value_type>(used_lookup_gates),
                nil::crypto3::marshalling::types::fill_plonk_lookup_tables<Endianness, typename ConstraintSystemType::lookup_tables_type::value_type>(used_lookup_tables)
    ));

    std::vector<std::uint8_t> cv;
    cv.resize(filled_val.length(), 0x00);
    auto write_iter = cv.begin();
    nil::marshalling::status_type status = filled_val.write(write_iter, cv.size());
    print_hex_byteblob(out, cv.cbegin(), cv.cend(), false);
}

enum class print_table_kind {
    PRIVATE,
    SHARED,
    FULL
};

enum class print_column_kind {
    WITNESS,
    SHARED,
    PUBLIC_INPUT,
    CONSTANT,
    SELECTOR
};

template<typename ValueType, typename ContainerType>
void fill_vector_value(std::vector<ValueType> &table_values, const ContainerType &table_col, std::uint32_t padding) {
        std::copy(table_col.begin(), table_col.end(), std::back_inserter(table_values));
        for(std::uint32_t j = table_col.size(); j < padding; j++){
            table_values.push_back(0);
        }
}

template<typename Endianness, typename ArithmetizationType, typename BlueprintFieldType>
void print_assignment_table(const assignment_proxy<ArithmetizationType> &table_proxy,
                            print_table_kind print_kind,
                            std::ostream &out = std::cout) {
    using AssignmentTableType = assignment_proxy<ArithmetizationType>;
    std::uint32_t usable_rows_amount;
    std::uint32_t total_columns;
    std::uint32_t shared_size = table_proxy.shareds_amount();
    std::uint32_t public_input_size = table_proxy.public_inputs_amount();
    std::uint32_t witness_size = table_proxy.witnesses_amount();
    std::uint32_t constant_size = table_proxy.constants_amount();
    std::uint32_t selector_size = table_proxy.selectors_amount();
    const auto lookup_constant_cols = table_proxy.get_lookup_constant_cols();
    const auto lookup_selector_cols = table_proxy.get_lookup_selector_cols();
    if (print_kind == print_table_kind::PRIVATE) {
        constant_size = constant_size - table_proxy.get_lookup_constant_amount();
        selector_size = selector_size - table_proxy.get_lookup_selector_amount();
        total_columns = witness_size + constant_size + selector_size;
        usable_rows_amount = table_proxy.get_used_rows().size();
    } else if (print_kind == print_table_kind::SHARED) {
        constant_size = table_proxy.get_lookup_constant_amount();
        selector_size = table_proxy.get_lookup_selector_amount();
        total_columns = shared_size + public_input_size + constant_size + selector_size;
        std::uint32_t max_shared_size = 0;
        std::uint32_t max_public_inputs_size = 0;
        std::uint32_t max_constant_size = 0;
        std::uint32_t max_selector_size = 0;
        for (std::uint32_t i = 0; i < shared_size; i++) {
            max_shared_size = std::max(max_shared_size, table_proxy.shared_column_size(i));
        }
        for (std::uint32_t i = 0; i < public_input_size; i++) {
            max_public_inputs_size = std::max(max_public_inputs_size, table_proxy.public_input_column_size(i));
        }
        for (const auto &i : lookup_constant_cols) {
            max_constant_size = std::max(max_constant_size, table_proxy.constant_column_size(i));
        }
        for (const auto &i : lookup_selector_cols) {
            max_selector_size = std::max(max_selector_size, table_proxy.selector_column_size(i));
        }
        usable_rows_amount = std::max({max_shared_size, max_public_inputs_size, max_constant_size, max_selector_size});
    } else { // FULL
        total_columns = AssignmentTableType::arithmetization_params::total_columns;
        std::uint32_t max_witness_size = 0;
        std::uint32_t max_public_inputs_size = 0;
        std::uint32_t max_constant_size = 0;
        std::uint32_t max_selector_size = 0;
        for (std::uint32_t i = 0; i < witness_size; i++) {
            max_witness_size = std::max(max_witness_size, table_proxy.witness_column_size(i));
        }
        for (std::uint32_t i = 0; i < public_input_size; i++) {
            max_public_inputs_size = std::max(max_public_inputs_size, table_proxy.public_input_column_size(i));
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

    using TTypeBase = nil::marshalling::field_type<Endianness>;
    using plonk_assignment_table = nil::marshalling::types::bundle<
        TTypeBase,
        std::tuple<
            nil::marshalling::types::integral<TTypeBase, std::size_t>, // usable_rows
            nil::marshalling::types::integral<TTypeBase, std::size_t>, // columns_number
            // table_values
            nil::marshalling::types::array_list<
                TTypeBase,
                nil::crypto3::marshalling::types::field_element<TTypeBase, typename AssignmentTableType::field_type::value_type>,
                nil::marshalling::option::sequence_size_field_prefix<nil::marshalling::types::integral<TTypeBase, std::size_t>>
            >
        >
    >;
    using column_type = typename crypto3::zk::snark::plonk_column<BlueprintFieldType>;

    std::vector<typename AssignmentTableType::field_type::value_type> table_values;
    if (print_kind == print_table_kind::FULL) {
        for (std::uint32_t i = 0; i < witness_size; i++) {
            fill_vector_value<typename AssignmentTableType::field_type::value_type, column_type>
                    (table_values, table_proxy.witness(i), padded_rows_amount);
        }
        for (std::uint32_t i = 0; i < public_input_size; i++) {
            fill_vector_value<typename AssignmentTableType::field_type::value_type, column_type>
                    (table_values, table_proxy.public_input(i), padded_rows_amount);
        }
        for (std::uint32_t i = 0; i < constant_size; i++) {
            fill_vector_value<typename AssignmentTableType::field_type::value_type, column_type>
                    (table_values, table_proxy.constant(i), padded_rows_amount);
        }
        for (std::uint32_t i = 0; i < selector_size; i++) {
            fill_vector_value<typename AssignmentTableType::field_type::value_type, column_type>
                    (table_values, table_proxy.selector(i), padded_rows_amount);
        }
    } else if (print_kind == print_table_kind::SHARED) {
        for (std::uint32_t i = 0; i < public_input_size; i++) {
            fill_vector_value<typename AssignmentTableType::field_type::value_type, column_type>
                    (table_values, table_proxy.public_input(i), padded_rows_amount);
        }
        for (std::uint32_t i = 0; i < shared_size; i++) {
            fill_vector_value<typename AssignmentTableType::field_type::value_type, column_type>
                    (table_values, table_proxy.shared(i), padded_rows_amount);
        }
        for (const auto &i : table_proxy.get_lookup_constant_cols()) {
            fill_vector_value<typename AssignmentTableType::field_type::value_type, column_type>
                    (table_values, table_proxy.constant(i), padded_rows_amount);
        }
        for (const auto &i : table_proxy.get_lookup_selector_cols()) {
            fill_vector_value<typename AssignmentTableType::field_type::value_type, column_type>
                    (table_values, table_proxy.selector(i), padded_rows_amount);
        }
    } else {
        const std::uint32_t padding = padded_rows_amount - usable_rows_amount;
        const auto& rows = table_proxy.get_used_rows();

        std::vector<std::uint32_t> constant_cols;
        const auto& lookup_constant_cols = table_proxy.get_lookup_constant_cols();
        for (std::uint32_t i = 0; i < table_proxy.constants_amount(); i++) {
            if (lookup_constant_cols.find(i) == lookup_constant_cols.end()) {
                constant_cols.push_back(i);
            }
        }
        std::vector<std::uint32_t> selector_cols;
        const auto& lookup_selector_cols = table_proxy.get_lookup_selector_cols();
        for (std::uint32_t i = 0; i < table_proxy.selectors_amount(); i++) {
            if (lookup_selector_cols.find(i) == lookup_selector_cols.end()) {
                selector_cols.push_back(i);
            }
        }
        for( std::size_t i = 0; i < AssignmentTableType::arithmetization_params::witness_columns; i++ ){
            const auto column_size = table_proxy.witness_column_size(i);
            for(const auto& j : rows){
                if (j < column_size) {
                    table_values.push_back(table_proxy.witness(i, j));
                } else {
                    table_values.push_back(0);
                }
            }
            for(std::uint32_t j = 0; j < padding; j++){
                table_values.push_back(0);
            }
        }
        for(const auto& i : constant_cols) {
            const auto column_size = table_proxy.constant_column_size(i);
            for(const auto& j : rows){
                if (j < column_size) {
                    table_values.push_back(table_proxy.constant(i, j));
                } else {
                    table_values.push_back(0);
                }
            }
            for(std::uint32_t j = 0; j < padding; j++){
                table_values.push_back(0);
            }
        }
        for(const auto& i : selector_cols) {
            const auto column_size = table_proxy.selector_column_size(i);
            for(const auto& j : rows) {
                if (j < column_size) {
                    table_values.push_back(table_proxy.selector(i, j));
                } else {
                    table_values.push_back(0);
                }
            }
            for(std::uint32_t j = 0; j < padding; j++){
                table_values.push_back(0);
            }
        }
    }

    auto filled_val = plonk_assignment_table(std::make_tuple(
            nil::marshalling::types::integral<TTypeBase, std::size_t>(usable_rows_amount),
            nil::marshalling::types::integral<TTypeBase, std::size_t>(total_columns),
            nil::crypto3::marshalling::types::fill_field_element_vector<typename AssignmentTableType::field_type::value_type, Endianness>(table_values)
    ));

    std::vector<std::uint8_t> cv;
    cv.resize(filled_val.length(), 0x00);
    auto write_iter = cv.begin();
    nil::marshalling::status_type status = filled_val.write(write_iter, cv.size());
    print_hex_byteblob(out, cv.cbegin(), cv.cend(), false);
}

template<typename CurveType, bool PrintCircuitOutput>
int curve_dependent_main(std::string bytecode_file_name,
                          std::string public_input_file_name,
                          std::string assignment_table_file_name,
                          std::string circuit_file_name,
                          long stack_size,
                          bool check_validity,
                          boost::log::trivial::severity_level log_level,
                          const std::string &policy,
                          std::uint32_t max_num_provers) {
    using BlueprintFieldType = typename CurveType::base_field_type;

    constexpr std::size_t WitnessColumns = 15;
    constexpr std::size_t PublicInputColumns = 1;
    constexpr std::size_t ConstantColumns = 30;
    constexpr std::size_t SelectorColumns = 35;

    using ArithmetizationParams =
        zk::snark::plonk_arithmetization_params<WitnessColumns, PublicInputColumns, ConstantColumns, SelectorColumns>;
    using ConstraintSystemType = zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;
    using ConstraintSystemProxyType = zk::snark::plonk_table<BlueprintFieldType, ArithmetizationParams, zk::snark::plonk_column<BlueprintFieldType>>;
    using ArithmetizationType =
            crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;
    using AssignmentTableType = zk::snark::plonk_table<BlueprintFieldType, ArithmetizationParams, zk::snark::plonk_column<BlueprintFieldType>>;

    std::vector<typename BlueprintFieldType::value_type> public_input;
    std::ifstream input_file(public_input_file_name.c_str());
    if (!input_file.is_open()) {
        std::cerr << "Could not open the file - '" << public_input_file_name << "'" << std::endl;
        return EXIT_FAILURE;
    }

    boost::json::stream_parser p;
    boost::json::error_code ec;
    while (!input_file.eof()) {
        char input_string[256];
        input_file.read(input_string, sizeof(input_string) - 1);
        input_string[input_file.gcount()] = '\0';
        p.write(input_string, ec);
        if (ec) {
            std::cerr << "JSON parsing of public input failed" << std::endl;
            return 1;
        }
    }
    p.finish(ec);
    if (ec) {
        std::cerr << "JSON parsing of public input failed" << std::endl;
        return 1;
    }
    boost::json::value input_json_value = p.release();
    if (!input_json_value.is_array()) {
        std::cerr << "Array of arguments is expected in JSON file" << std::endl;
        return 1;
    }

    nil::blueprint::parser<BlueprintFieldType, ArithmetizationParams, PrintCircuitOutput> parser_instance(stack_size,
                                                                                                          log_level, max_num_provers, policy);

    std::unique_ptr<llvm::Module> module = parser_instance.parseIRFile(bytecode_file_name.c_str());
    if (module == nullptr) {
        return 1;
    }

    if (!parser_instance.evaluate(*module, input_json_value.as_array())) {
        return 1;
    }

    ASSERT_MSG(!parser_instance.assignments.empty() && !parser_instance.circuits.empty(), "Not found any proxy for prover" );

    // pack lookup tables
    if (parser_instance.circuits[0].get_reserved_tables().size() > 0) {
        std::vector <std::size_t> lookup_columns_indices;
        const auto& lookup_tables = parser_instance.circuits[0].get_reserved_tables();
        const std::uint32_t max_usable_rows = 500000;
        // looking for free constant columns
        std::uint32_t max_used_col = 0;
        for(std::size_t i = 0; i < ConstantColumns; i++) {
            if(parser_instance.assignments[0].constant_column_size(i) != 0) {
                max_used_col = i;
            }
        }
        lookup_columns_indices.resize(ConstantColumns - max_used_col - 1);
        std::iota(lookup_columns_indices.begin(), lookup_columns_indices.end(), max_used_col + 1); // fill max_used_col + 1, max_used_col + 2, ...

        auto usable_rows_amount = zk::snark::pack_lookup_tables_horizontal(
                parser_instance.circuits[0].get_reserved_indices(),
                parser_instance.circuits[0].get_reserved_tables(),
                parser_instance.circuits[0].get(),
                parser_instance.assignments[0].get(),
                lookup_columns_indices,
                parser_instance.assignments[0].allocated_rows(),
                max_usable_rows
        );
    }

    // print shared table
    std::ofstream shared_otable;
    const auto print_kind = parser_instance.assignments.size() > 1 ? print_table_kind::SHARED : print_table_kind::FULL;
    std::string shared_otable_file_name = parser_instance.assignments.size() > 1 ?
                            assignment_table_file_name + "_shared" : assignment_table_file_name;
    shared_otable.open(shared_otable_file_name);
    if (!shared_otable) {
        std::cout << "Something wrong with output " << shared_otable_file_name << std::endl;
        return 1;
    }

    print_assignment_table<
            nil::marshalling::option::big_endian,
            ArithmetizationType, BlueprintFieldType
    >(parser_instance.assignments[0], print_kind, shared_otable);

    shared_otable.close();

    if (parser_instance.assignments.size() > 1) {
        for (const auto &it: parser_instance.assignments) {
            std::ofstream otable;
            otable.open(assignment_table_file_name + std::to_string(it.get_id()));
            if (!otable) {
                std::cout << "Something wrong with output " << assignment_table_file_name + std::to_string(it.get_id()) << std::endl;
                return 1;
            }

            print_assignment_table<
                    nil::marshalling::option::big_endian,
                    ArithmetizationType, BlueprintFieldType
            >(it, print_table_kind::PRIVATE, otable);


//    nil::blueprint::profiling_assignment_table(parser_instance.assignmnt, desc.usable_rows_amount, otable);
            otable.close();
        }
    }
    auto assignment_it = parser_instance.assignments.begin();
    for (auto& it : parser_instance.circuits) {
        std::ofstream ocircuit;
        std::string file_name = parser_instance.circuits.size() > 1 ?
                                circuit_file_name + std::to_string(it.get_id()) : circuit_file_name;
        ocircuit.open(file_name);
        if (!ocircuit) {
            std::cout << "Something wrong with output " << file_name << std::endl;
            return 1;
        }
        ASSERT_MSG(assignment_it != parser_instance.assignments.end(), "Not found assignment for circuit");
        print_circuit<nil::marshalling::option::big_endian, ArithmetizationType, ConstraintSystemType>
                (it, *assignment_it, (parser_instance.assignments.size() > 1), ocircuit);
        ocircuit.close();
        assignment_it++;
    }

    if (check_validity){
        ASSERT_MSG(nil::blueprint::is_satisfied(parser_instance.circuits[0].get(), parser_instance.assignments[0].get()),
                   "The circuit is not satisfied");
        auto assignment_it = parser_instance.assignments.begin();
        for (auto& it : parser_instance.circuits) {
            ASSERT_MSG(assignment_it != parser_instance.assignments.end(), "Not found assignment for circuit" );
            assignment_it->set_check(true);
            bool is_accessible = nil::blueprint::is_accessible(it, *assignment_it);
            assignment_it->set_check(false);
            ASSERT_MSG(is_accessible, ("The circuit is not satisfied on prover " + std::to_string(it.get_id())).c_str() );
            assignment_it++;
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
            ("assignment-table,t", boost::program_options::value<std::string>(), "Assignment table output file")
            ("circuit,c", boost::program_options::value<std::string>(), "Circuit output file")
            ("elliptic-curve-type,e", boost::program_options::value<std::string>(), "Native elliptic curve type (pallas, vesta, ed25519, bls12-381)")
            ("stack-size,s", boost::program_options::value<long>(), "Stack size in bytes")
            ("check", "Check satisfiability of the generated circuit")
            ("log-level,l", boost::program_options::value<std::string>(), "Log level (trace, debug, info, warning, error, fatal)")
            ("print_circuit_output", "print output of the circuit")
            ("policy", boost::program_options::value<std::string>(), "Policy for creating circuits. Possible values: default")
            ("max-num-provers", boost::program_options::value<int>(), "Maximum number of provers. Possible values >= 1");
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
        std::cout << "undefined" << std::endl;
#endif
        return 0;
    }

    std::string bytecode_file_name;
    std::string public_input_file_name;
    std::string assignment_table_file_name;
    std::string circuit_file_name;
    std::string elliptic_curve;
    std::string log_level;
    long stack_size;

    if (vm.count("bytecode")) {
        bytecode_file_name = vm["bytecode"].as<std::string>();
    } else {
        std::cerr << "Invalid command line argument - bytecode file name is not specified" << std::endl;
        std::cout << options_desc << std::endl;
        return 1;
    }

    if (vm.count("public-input")) {
        public_input_file_name = vm["public-input"].as<std::string>();
    } else {
        std::cerr << "Invalid command line argument - public input file name is not specified" << std::endl;
        std::cout << options_desc << std::endl;
        return 1;
    }

    if (vm.count("assignment-table")) {
        assignment_table_file_name = vm["assignment-table"].as<std::string>();
    } else {
        std::cerr << "Invalid command line argument - assignment table file name is not specified" << std::endl;
        std::cout << options_desc << std::endl;
        return 1;
    }

    if (vm.count("circuit")) {
        circuit_file_name = vm["circuit"].as<std::string>();
    } else {
        std::cerr << "Invalid command line argument - circuit file name is not specified" << std::endl;
        std::cout << options_desc << std::endl;
        return 1;
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
        {"bls12-381", 3},
    };

    if (curve_options.find(elliptic_curve) == curve_options.end()) {
        std::cerr << "Invalid command line argument -e (Native elliptic curve type): " << elliptic_curve << std::endl;
        std::cout << options_desc << std::endl;
        return 1;
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

    switch (curve_options[elliptic_curve]) {
        case 0: {
            if (vm.count("print_circuit_output")) {
                return curve_dependent_main<typename algebra::curves::pallas, true>(
                                                                          bytecode_file_name,
                                                                          public_input_file_name,
                                                                          assignment_table_file_name,
                                                                          circuit_file_name,
                                                                          stack_size,
                                                                          vm.count("check"),
                                                                          log_options[log_level],
                                                                          policy,
                                                                          max_num_provers);
            }
            else {
                return curve_dependent_main<typename algebra::curves::pallas, false>(
                                                                          bytecode_file_name,
                                                                          public_input_file_name,
                                                                          assignment_table_file_name,
                                                                          circuit_file_name,
                                                                          stack_size,
                                                                          vm.count("check"),
                                                                          log_options[log_level],
                                                                          policy,
                                                                          max_num_provers);
            }
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
            UNREACHABLE("bls12-381 curve based circuits are not supported yet");
            break;
        }
    };

    return 0;
}
