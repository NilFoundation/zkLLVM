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

#include <future>
#include <thread>
#include <chrono>
#include <filesystem>

using namespace nil;
using namespace nil::crypto3;
using namespace nil::blueprint;

template<typename Endianness, typename ArithmetizationType, typename ConstraintSystemType>
void print_circuit(const circuit_proxy<ArithmetizationType> &circuit_proxy,
                   const assignment_proxy<ArithmetizationType> &table_proxy,
                   bool multi_prover, std::uint32_t idx, std::ostream &out = std::cout) {
    using TTypeBase = nil::marshalling::field_type<Endianness>;
    using value_marshalling_type =
        nil::crypto3::marshalling::types::plonk_constraint_system<TTypeBase, ConstraintSystemType>;
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

    if (multi_prover && idx > 0) {
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


    // fill public input sizes
    nil::crypto3::marshalling::types::public_input_sizes_type<TTypeBase> public_input_sizes;
    using public_input_size_type = typename nil::crypto3::marshalling::types::public_input_sizes_type<TTypeBase>::element_type;
    const auto public_input_size = table_proxy.public_inputs_amount();
    for (std::uint32_t i = 0; i < public_input_size; i++) {
        public_input_sizes.value().push_back(public_input_size_type(table_proxy.public_input_column_size(i)));
    }
    if (multi_prover) {
        public_input_sizes.value().push_back(public_input_size_type(table_proxy.shared_column_size(0)));
    }

    auto filled_val =
        value_marshalling_type(std::make_tuple(
            nil::crypto3::marshalling::types::fill_plonk_gates<Endianness, typename ConstraintSystemType::gates_container_type::value_type>(used_gates),
            nil::crypto3::marshalling::types::fill_plonk_copy_constraints<Endianness, typename ConstraintSystemType::field_type>(used_copy_constraints),
            nil::crypto3::marshalling::types::fill_plonk_lookup_gates<Endianness, typename ConstraintSystemType::lookup_gates_container_type::value_type>(used_lookup_gates),
            nil::crypto3::marshalling::types::fill_plonk_lookup_tables<Endianness, typename ConstraintSystemType::lookup_tables_type::value_type>(used_lookup_tables),
            public_input_sizes
    ));

    std::vector<std::uint8_t> cv;
    cv.resize(filled_val.length(), 0x00);
    auto cv_iter = cv.begin();
    nil::marshalling::status_type status = filled_val.write(cv_iter, cv.size());
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

template<typename Endianness>
void print_size_t(
    std::size_t input,
    std::ostream &out
) {
    using TTypeBase = nil::marshalling::field_type<Endianness>;
    auto integer_container = nil::marshalling::types::integral<TTypeBase, std::size_t>(input);
    std::array<std::uint8_t, integer_container.length()> char_array{};
    auto write_iter = char_array.begin();
    ASSERT(integer_container.write(write_iter, char_array.size()) == nil::marshalling::status_type::success);
    out.write(reinterpret_cast<char*>(char_array.data()), char_array.size());
}

template<typename Endianness, typename ArithmetizationType>
inline void print_zero_field(
    std::ostream &out
) {
    using TTypeBase = nil::marshalling::field_type<Endianness>;
    using AssignmentTableType = assignment_proxy<ArithmetizationType>;
    using field_element = nil::crypto3::marshalling::types::field_element<
        TTypeBase, typename AssignmentTableType::field_type::value_type>;
    std::array<std::uint8_t, field_element().length()> array{};
    out.write(reinterpret_cast<char*>(array.data()), array.size());
}


template<typename Endianness, typename ArithmetizationType>
void print_field(
    const typename assignment_proxy<ArithmetizationType>::field_type::value_type &input,
    std::ostream &out
) {
    using TTypeBase = nil::marshalling::field_type<Endianness>;
    using AssignmentTableType = assignment_proxy<ArithmetizationType>;
    auto field_container = nil::crypto3::marshalling::types::field_element<TTypeBase, typename AssignmentTableType::field_type::value_type>(input);
    std::array<std::uint8_t, field_container.length()> char_array{};
    auto write_iter = char_array.begin();
    ASSERT(field_container.write(write_iter, char_array.size()) == nil::marshalling::status_type::success);
    out.write(reinterpret_cast<char*>(char_array.data()), char_array.size());
}

template<typename Endianness, typename ArithmetizationType, typename ContainerType>
void print_vector_value(
    const std::size_t padded_rows_amount,
    const ContainerType &table_col,
    std::ostream &out
) {
    for (std::size_t i = 0; i < padded_rows_amount; i++) {
        if (i < table_col.size()) {
            print_field<Endianness, ArithmetizationType>(table_col[i], out);
        } else {
            print_zero_field<Endianness, ArithmetizationType>(out);
        }
    }
}

template<typename Endianness, typename ArithmetizationType, typename BlueprintFieldType>
void print_selectors(const assignment_proxy<ArithmetizationType> &table_proxy,
                            print_table_kind print_kind,
                            std::size_t padded_rows_amount,
                            std::uint32_t ComponentSelectorColumns,
                            std::ostream &out = std::cout) {
    std::uint32_t selector_size = table_proxy.selectors_amount();

    using column_type = typename crypto3::zk::snark::plonk_column<BlueprintFieldType>;

    print_size_t<Endianness>(selector_size * padded_rows_amount, out);
    if (print_kind == print_table_kind::SINGLE_PROVER) {
        for (std::uint32_t i = 0; i < selector_size; i++) {
            print_vector_value<Endianness, ArithmetizationType, column_type>(padded_rows_amount, table_proxy.selector(i), out);
        }
    } else {
        const auto& rows = table_proxy.get_used_rows();
        const auto& selector_rows = table_proxy.get_used_selector_rows();

        std::uint32_t selector_idx = 0;
        for (std::uint32_t i = 0; i < ComponentSelectorColumns; i++) {
            const auto column_size = table_proxy.selector_column_size(i);
            std::uint32_t offset = 0;
            for(const auto& j : rows){
                if (j < column_size) {
                    if (selector_rows.find(j) != selector_rows.end()) {
                        print_field<Endianness, ArithmetizationType>(table_proxy.selector(i, j), out);
                    } else {
                        print_zero_field<Endianness, ArithmetizationType>(out);
                    }
                    offset++;
                }
            }
            ASSERT(offset < padded_rows_amount);
            while(offset < padded_rows_amount) {
                print_zero_field<Endianness, ArithmetizationType>(out);
                offset++;
            }

            selector_idx += padded_rows_amount;
        }

        for (std::uint32_t i = ComponentSelectorColumns; i < selector_size; i++) {
            print_vector_value<Endianness, ArithmetizationType, column_type>(padded_rows_amount, table_proxy.selector(i), out);
            selector_idx += padded_rows_amount;
        }
    }
}

template<typename Endianness, typename ArithmetizationType, typename BlueprintFieldType>
void print_constants(const assignment_proxy<ArithmetizationType> &table_proxy,
                            print_table_kind print_kind,
                            std::size_t padded_rows_amount,
                            std::uint32_t ComponentConstantColumns,
                            std::ostream &out = std::cout) {
    std::uint32_t constant_size = table_proxy.constants_amount();

    using column_type = typename crypto3::zk::snark::plonk_column<BlueprintFieldType>;

    print_size_t<Endianness>(constant_size * padded_rows_amount, out);
    if (print_kind == print_table_kind::SINGLE_PROVER) {
        for (std::uint32_t i = 0; i < constant_size; i++) {
            print_vector_value<Endianness, ArithmetizationType, column_type>(padded_rows_amount, table_proxy.constant(i), out);
        }
    } else {
        const auto& rows = table_proxy.get_used_rows();
        std::uint32_t constant_idx = 0;
        for (std::uint32_t i = 0; i < ComponentConstantColumns; i++) {
            const auto column_size = table_proxy.constant_column_size(i);
            std::uint32_t offset = 0;
            for(const auto& j : rows){
                if (j < column_size) {
                    print_field<Endianness, ArithmetizationType>(table_proxy.constant(i, j), out);
                    offset++;
                }
            }
            ASSERT(offset < padded_rows_amount);
            while(offset < padded_rows_amount) {
                print_zero_field<Endianness, ArithmetizationType>(out);
                offset++;
            }

            constant_idx += padded_rows_amount;
        }

        for (std::uint32_t i = ComponentConstantColumns; i < constant_size; i++) {
            print_vector_value<Endianness, ArithmetizationType, column_type>(padded_rows_amount, table_proxy.constant(i), out);
            constant_idx += padded_rows_amount;
        }
    }
}

template<typename Endianness, typename ArithmetizationType, typename BlueprintFieldType>
void print_pub_inp(const assignment_proxy<ArithmetizationType> &table_proxy,
                            print_table_kind print_kind,
                            std::size_t padded_rows_amount,
                            std::ostream &out = std::cout) {
    std::uint32_t shared_size = (print_kind == print_table_kind::MULTI_PROVER) ? 1 : 0;
    std::uint32_t public_input_size = table_proxy.public_inputs_amount();

    using column_type = typename crypto3::zk::snark::plonk_column<BlueprintFieldType>;

    print_size_t<Endianness>((public_input_size + shared_size) * padded_rows_amount, out);
    if (print_kind == print_table_kind::SINGLE_PROVER) {
        for (std::uint32_t i = 0; i < public_input_size; i++) {
            print_vector_value<Endianness, ArithmetizationType, column_type>(padded_rows_amount, table_proxy.public_input(i), out);
        }
    } else {

        std::uint32_t pub_inp_idx = 0;
        for (std::uint32_t i = 0; i < public_input_size; i++) {
            print_vector_value<Endianness, ArithmetizationType, column_type>(padded_rows_amount, table_proxy.public_input(i), out);
            pub_inp_idx += padded_rows_amount;
        }
        for (std::uint32_t i = 0; i < shared_size; i++) {
            print_vector_value<Endianness, ArithmetizationType, column_type>(padded_rows_amount, table_proxy.shared(i), out);
            pub_inp_idx += padded_rows_amount;
        }
    }
}

template<typename Endianness, typename ArithmetizationType, typename BlueprintFieldType>
void print_witness(const assignment_proxy<ArithmetizationType> &table_proxy,
                            print_table_kind print_kind,
                            std::size_t padded_rows_amount,
                            std::ostream &out = std::cout) {
    std::uint32_t witness_size = table_proxy.witnesses_amount();

    using column_type = typename crypto3::zk::snark::plonk_column<BlueprintFieldType>;

    print_size_t<Endianness>(witness_size * padded_rows_amount, out);
    if (print_kind == print_table_kind::SINGLE_PROVER) {
        for (std::uint32_t i = 0; i < witness_size; i++) {
            print_vector_value<Endianness, ArithmetizationType, column_type>(padded_rows_amount, table_proxy.witness(i), out);
        }
    } else {
        const auto& rows = table_proxy.get_used_rows();
        std::uint32_t witness_idx = 0;
        for( std::size_t i = 0; i < witness_size; i++ ){
            const auto column_size = table_proxy.witness_column_size(i);
            std::uint32_t offset = 0;
            for(const auto& j : rows){
                if (j < column_size) {
                    print_field<Endianness, ArithmetizationType>(table_proxy.witness(i, j), out);
                    offset++;
                }
            }
            ASSERT(offset < padded_rows_amount);
            while(offset < padded_rows_amount) {
                print_zero_field<Endianness, ArithmetizationType>(out);
                offset++;
            }
            witness_idx += padded_rows_amount;
        }
    }
}

std::string add_filename_prefix(
    const std::string& prefix,
    const std::string& file_name
) {
    std::filesystem::path path(file_name);
    std::filesystem::path parent_path = path.parent_path();
    std::filesystem::path filename = path.filename();

    std::string new_filename = prefix + filename.string();
    std::filesystem::path new_path = parent_path / new_filename;

    return new_path.string();
}


template<typename Endianness, typename ArithmetizationType, typename BlueprintFieldType>
std::uint32_t padded_rows_from_table_header(
    const assignment_proxy<ArithmetizationType> &table_proxy,
    print_table_kind print_kind,
    std::string assignment_table_file_name,
    std::uint32_t idx
) {
    if (print_kind == print_table_kind::MULTI_PROVER) {
        assignment_table_file_name = assignment_table_file_name  + std::to_string(idx);
    }
    std::ifstream itable_header;
    itable_header.open(add_filename_prefix("header_", assignment_table_file_name), std::ios_base::binary | std::ios_base::in);
    if (!itable_header) {
        throw std::runtime_error("Something wrong with input file header_" + assignment_table_file_name); // TODO: add catch
    }
    using TTypeBase = nil::marshalling::field_type<Endianness>;
    using table_header_marshalling_type = nil::crypto3::marshalling::types::table_header_type<TTypeBase>;

    table_header_marshalling_type marshalled_table_header;
    std::vector<std::uint8_t> header_binary;
    itable_header.seekg(0, std::ios_base::end);
    const auto header_size = itable_header.tellg();
    header_binary.resize(header_size);
    itable_header.seekg(0, std::ios_base::beg);
    itable_header.read(reinterpret_cast<char*>(header_binary.data()), header_size);
    itable_header.close();
    auto hiter = header_binary.begin();
    auto status = marshalled_table_header.read(hiter, header_binary.size());
    ASSERT(status == nil::marshalling::status_type::success);

    std::uint32_t padded_rows_amount = std::get<5>(marshalled_table_header.value()).value();
    return padded_rows_amount;

}

template<typename Endianness, typename ArithmetizationType, typename BlueprintFieldType>
void print_assignment_table_dynamic(
    std::uint32_t padded_rows_amount,
    const assignment_proxy<ArithmetizationType> &table_proxy,
    print_table_kind print_kind,
    std::string assignment_table_file_name,
    std::uint32_t idx
) {

    if (print_kind == print_table_kind::MULTI_PROVER) {
        assignment_table_file_name = assignment_table_file_name  + std::to_string(idx);
    }

    std::ofstream otable_witness;
    std::ofstream otable_pub_inp;

    otable_witness.open(add_filename_prefix("witness_", assignment_table_file_name), std::ios_base::binary | std::ios_base::out);
    if (!otable_witness) {
        throw std::runtime_error("Something wrong with output " + add_filename_prefix("witness_", assignment_table_file_name));
    }
    otable_pub_inp.open(add_filename_prefix("pub_inp_", assignment_table_file_name), std::ios_base::binary | std::ios_base::out);
    if (!otable_pub_inp) {
        throw std::runtime_error("Something wrong with output " + add_filename_prefix("pub_inp_", assignment_table_file_name));
    }

    print_witness<Endianness, ArithmetizationType,BlueprintFieldType>(table_proxy, print_kind, padded_rows_amount, otable_witness);
    print_pub_inp<Endianness, ArithmetizationType,BlueprintFieldType>(table_proxy, print_kind, padded_rows_amount, otable_pub_inp);
}


template<typename ArithmetizationType>
std::array<std::uint32_t, 7> extract_table_parameters_from_table(
    const assignment_proxy<ArithmetizationType> &table_proxy,
    print_table_kind print_kind
) {

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

    std::array<std::uint32_t, 7> result = {
        witness_size,
        public_input_size,
        shared_size,
        constant_size,
        selector_size,
        usable_rows_amount,
        padded_rows_amount
    };

    return result;
}


template<typename Endianness, typename ArithmetizationType, typename BlueprintFieldType>
void print_assignment_table_static(
    std::array<std::uint32_t, 7> asdf,
    const assignment_proxy<ArithmetizationType> &table_proxy,
    print_table_kind print_kind,
    std::uint32_t ComponentConstantColumns, std::uint32_t ComponentSelectorColumns,
    std::string assignment_table_file_name,
    std::uint32_t idx
 ) {

    if (print_kind == print_table_kind::MULTI_PROVER) {
        assignment_table_file_name = assignment_table_file_name  + std::to_string(idx);
    }

    std::ofstream otable_header;
    std::ofstream otable_constants;
    std::ofstream otable_selectors;

    otable_header.open(add_filename_prefix("header_", assignment_table_file_name), std::ios_base::binary | std::ios_base::out);
    if (!otable_header)
        throw std::runtime_error("Something wrong with output " + add_filename_prefix("header_", assignment_table_file_name));
    otable_constants.open(add_filename_prefix("constants_", assignment_table_file_name), std::ios_base::binary | std::ios_base::out);
    if (!otable_constants)
        throw std::runtime_error("Something wrong with output " + add_filename_prefix("constants_", assignment_table_file_name));
    otable_selectors.open(add_filename_prefix("selectors_", assignment_table_file_name), std::ios_base::binary | std::ios_base::out);
    if (!otable_selectors)
        throw std::runtime_error("Something wrong with output " + add_filename_prefix("selectors_", assignment_table_file_name));


    using AssignmentTableType = assignment_proxy<ArithmetizationType>;

    std::uint32_t witness_size = asdf[0];
    std::uint32_t public_input_size = asdf[1];
    std::uint32_t shared_size = asdf[2];
    std::uint32_t constant_size = asdf[3];
    std::uint32_t selector_size = asdf[4];
    std::uint32_t usable_rows_amount = asdf[5];
    std::uint32_t padded_rows_amount = asdf[6];

    print_size_t<Endianness>(witness_size,                    otable_header);
    print_size_t<Endianness>(public_input_size + shared_size, otable_header);
    print_size_t<Endianness>(constant_size,                   otable_header);
    print_size_t<Endianness>(selector_size,                   otable_header);
    print_size_t<Endianness>(usable_rows_amount,              otable_header);
    print_size_t<Endianness>(padded_rows_amount,              otable_header);

    print_constants<Endianness, ArithmetizationType,BlueprintFieldType>(table_proxy, print_kind, padded_rows_amount, ComponentConstantColumns, otable_constants);
    print_selectors<Endianness, ArithmetizationType,BlueprintFieldType>(table_proxy, print_kind, padded_rows_amount, ComponentSelectorColumns, otable_selectors);
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

template<typename ArithmetizationType, typename BlueprintFieldType>
void assignment_table_printer(
    std::string assignment_table_file_name,
    std::uint32_t idx,
    nil::blueprint::assigner<BlueprintFieldType> &assigner_instance,
    const std::size_t &ComponentConstantColumns,
    const std::size_t &ComponentSelectorColumns
) {
    std::array<std::uint32_t, 7> asdf = extract_table_parameters_from_table(
        assigner_instance.assignments[idx],
        print_table_kind::MULTI_PROVER
    );

    print_assignment_table_dynamic<nil::marshalling::option::big_endian, ArithmetizationType, BlueprintFieldType>(
        asdf[6],
        assigner_instance.assignments[idx],
        print_table_kind::MULTI_PROVER,
        assignment_table_file_name,
        idx
    );
}

template<typename BlueprintFieldType>
int curve_dependent_main(std::string bytecode_file_name,
                          std::string public_input_file_name,
                          std::string private_input_file_name,
                          std::string assignment_table_file_name,
                          std::string circuit_file_name,
                          std::string table_pieces_file_name,
                          std::string processed_public_input_file_name,
                          long stack_size,
                          bool check_validity,
                          boost::log::trivial::severity_level log_level,
                          const std::string &policy,
                          nil::blueprint::generation_mode gen_mode,
                          std::uint32_t max_num_provers,
                          std::uint32_t max_lookup_rows,
                          std::uint32_t target_prover,
                          nil::blueprint::print_format circuit_output_print_format,
                          std::array<std::size_t, 6> column_sizes
                          ) {

    ASSERT_MSG(!(check_validity && gen_mode.has_fast_tbl()), "validity check does not work in fast table generation mode!");

    const std::size_t ComponentConstantColumns = column_sizes[2];
    const std::size_t LookupConstantColumns = column_sizes[3];
    const std::size_t ComponentSelectorColumns = column_sizes[4];
    const std::size_t LookupSelectorColumns = column_sizes[5];

    const std::size_t WitnessColumns = column_sizes[0];
    const std::size_t PublicInputColumns = column_sizes[1];

    const std::size_t ConstantColumns = ComponentConstantColumns + LookupConstantColumns;
    const std::size_t SelectorColumns = ComponentSelectorColumns + LookupSelectorColumns;

    zk::snark::plonk_table_description<BlueprintFieldType> desc(
        WitnessColumns, PublicInputColumns, ConstantColumns, SelectorColumns);

    using ConstraintSystemType = zk::snark::plonk_constraint_system<BlueprintFieldType>;
    using ConstraintSystemProxyType = zk::snark::plonk_table<BlueprintFieldType, zk::snark::plonk_column<BlueprintFieldType>>;
    using ArithmetizationType =
            crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType>;
    using AssignmentTableType = zk::snark::plonk_table<BlueprintFieldType, zk::snark::plonk_column<BlueprintFieldType>>;

    using Endianness = nil::marshalling::option::big_endian;
    using TTypeBase = nil::marshalling::field_type<Endianness>;
    using constant_column_marshalling_type =
        nil::marshalling::types::array_list<
            TTypeBase,
            nil::crypto3::marshalling::types::field_element<
                TTypeBase,
                typename BlueprintFieldType::value_type>,
                nil::marshalling::option::sequence_size_field_prefix<
                    nil::marshalling::types::integral<TTypeBase, std::size_t>
                >
            >;

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

    using var = crypto3::zk::snark::plonk_variable<typename AssignmentTableType::field_type::value_type>;

    std::vector<table_piece<var>> table_pieces = {}; // we create table pieces in any case and pass into assigner by reference
    std::vector<std::vector<std::uint32_t>> all_used_rows;
    int provers_amount;
    std::vector<std::pair<std::uint32_t, var>> to_be_shared;

    if (gen_mode.has_fast_tbl()) { // if we are generating tables in a fast way then need to parse table_pieces from file

        std::ifstream inp_json(table_pieces_file_name);

        if (!inp_json.is_open()) {
            std::cerr << "unable to open table_pieces file" << std::endl;
            return 1;
        }

        std::string str((std::istreambuf_iterator<char>(inp_json)),
                        std::istreambuf_iterator<char>());
        auto parsed = boost::json::parse(str);
        boost::json::object obj = parsed.as_object();

        boost::json::array arr = obj["table_pieces"].as_array();
        for (const auto& item : arr) {
            table_pieces.emplace_back(item.as_object());
        }

        provers_amount = obj["provers_amount"].as_int64();

        boost::json::array used_rows_json = obj["used_rows"].as_array();
        for (const auto& row : used_rows_json) {
            std::vector<std::uint32_t> current_used_rows = {};
            for (const auto& elem : row.as_array()) {
                current_used_rows.push_back(elem.as_int64());
            }
            all_used_rows.push_back(current_used_rows);
        }

        boost::json::array to_be_shared_json = obj["to_be_shared"].as_array();
        for (const auto& pair_json : to_be_shared_json) {
            boost::json::object pair_obj = pair_json.as_object();
            std::uint32_t first = pair_obj["first"].as_int64();
            var second = var(pair_obj["second"].as_object());
            to_be_shared.emplace_back(first, second);
        }
    }

    auto assigner_instance_creation_start = std::chrono::high_resolution_clock::now();
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
    auto assigner_instance_creation_duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - assigner_instance_creation_start);
    BOOST_LOG_TRIVIAL(debug) << "assigner_instance_creation_duration: " << assigner_instance_creation_duration.count() << "ms";

    auto parse_ir_file_start = std::chrono::high_resolution_clock::now();
    if (!assigner_instance.parse_ir_file(bytecode_file_name.c_str())) {
        return 1;
    }
    if (!processed_public_input_file_name.empty()) {
        if (!assigner_instance.dump_public_input(public_input_json_value.as_array(), processed_public_input_file_name)) {
            return 1;
        }
        return 0;
    }
    auto parse_ir_file_duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - parse_ir_file_start);
    BOOST_LOG_TRIVIAL(debug) << "parse_ir_file_duration: " << parse_ir_file_duration.count() << "ms";


    std::vector<std::vector<std::vector<typename BlueprintFieldType::value_type>>> all_constant_columns;

    if (gen_mode.has_fast_tbl()) {
        // parse constant columns

        for (std::size_t i = 0; i < provers_amount; i++) {
            std::ifstream iconstants;
            std::string table_file_name = provers_amount == 1 ? assignment_table_file_name : assignment_table_file_name + std::to_string(i);

            iconstants.open("constants_" + table_file_name, std::ios_base::binary | std::ios_base::in);
            if (!iconstants) {
                std::cout << "Cannot open " << "constants_" + table_file_name << std::endl;
                return 1;
            }
            std::vector<std::uint8_t> v;
            std::vector<std::uint8_t> consant_cols_vector;
            iconstants.seekg(0, std::ios_base::end);
            const auto fsize = iconstants.tellg();
            consant_cols_vector.resize(fsize);
            iconstants.seekg(0, std::ios_base::beg);
            iconstants.read(reinterpret_cast<char*>(consant_cols_vector.data()), fsize);
            if (!iconstants) {
                std::cout << "Cannot parse input file " << assignment_table_file_name << std::endl;
                return 1;
            }
            iconstants.close();
            constant_column_marshalling_type marshalled_constant_column_data;
            auto citer = consant_cols_vector.begin();
            auto const_column_marshalling_status =   marshalled_constant_column_data.read(citer, consant_cols_vector.size());
///
    std::ifstream itable_header;
    std::ofstream otable_witness;
    std::ofstream otable_pub_inp;
    itable_header.open(add_filename_prefix("header_", table_file_name),  std::ios_base::in);
    if (!itable_header) {
        throw std::runtime_error("Something wrong with input file header_" + table_file_name); // TODO: add catch
    }

    using TTypeBase = nil::marshalling::field_type<Endianness>;
    using table_header_marshalling_type = nil::crypto3::marshalling::types::table_header_type<TTypeBase>;

    table_header_marshalling_type marshalled_table_header;
    std::vector<std::uint8_t> header_binary;
    itable_header.seekg(0, std::ios_base::end);
    const auto header_size = itable_header.tellg();
    header_binary.resize(header_size);
    itable_header.seekg(0, std::ios_base::beg);
    itable_header.read(reinterpret_cast<char*>(header_binary.data()),      header_size);
    itable_header.close();
    auto hiter = header_binary.begin();
    auto status = marshalled_table_header.read(hiter, header_binary.size());
    ASSERT(status == nil::marshalling::status_type::success);

    std::uint32_t columns_rows_amount = std::get<5>(marshalled_table_header.value()).value();

///
            std::vector<std::vector<typename BlueprintFieldType::value_type>> constant_columns =
                nil::crypto3::marshalling::types::make_field_element_columns_vector<typename BlueprintFieldType::value_type, Endianness>
                (
                    marshalled_constant_column_data,
                    // we must extract desc from table_header, but for now its ok to use default values
                    desc.constant_columns, // TODO we should take it from header file, but it will almost always work okay without it
                    columns_rows_amount
                );
            all_constant_columns.emplace_back(constant_columns);
        }
    }


    auto parser_evaluation_start = std::chrono::high_resolution_clock::now();
    if (
        !assigner_instance.evaluate(
            public_input_json_value.as_array(),
            private_input_json_value.as_array(),
            all_constant_columns,
            all_used_rows,
            to_be_shared,
            table_pieces
        )
    ) {
        return 1;
    }
    auto parser_evaluation_duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - parser_evaluation_start);
    BOOST_LOG_TRIVIAL(debug) << "parser_evaluation_duration: " << parser_evaluation_duration.count() << "ms";

    ASSERT_MSG(!assigner_instance.assignments.empty() && !assigner_instance.circuits.empty(), "Not found any proxy for prover" );

    if (gen_mode.has_size_estimation()) {
        return 0;
    }

    if (gen_mode.has_circuit()) {

        boost::json::object top_level_json;
        top_level_json["provers_amount"] = assigner_instance.assignments.size();

        boost::json::array pieces_json;
        for (const auto& piece : assigner_instance.get_table_pieces()) {
            pieces_json.push_back(piece.to_json());
        }
        top_level_json["table_pieces"] = pieces_json;

        boost::json::array used_rows_json;
        for (std::size_t i = 0; i < assigner_instance.assignments.size(); i++) {
            boost::json::array current_row;
            for (const auto& r : assigner_instance.assignments[i].get_used_rows()) {
                current_row.push_back(r);
            }
            used_rows_json.push_back(current_row);
        }
        top_level_json["used_rows"] = used_rows_json;

        boost::json::array to_be_shared_json;

        for (const auto& pair : assigner_instance.get_to_be_shared()) {
            boost::json::object pair_json;
            pair_json["first"] = pair.first;
            pair_json["second"] = pair.second.to_json();
            to_be_shared_json.push_back(pair_json);
        }
        top_level_json["to_be_shared"] = to_be_shared_json;

        std::string serialized = boost::json::serialize(top_level_json);

        std::ofstream file(table_pieces_file_name);
        file << serialized;
        file.close();
    }

    if (!gen_mode.has_fast_tbl()) {
    auto pack_lookup_start = std::chrono::high_resolution_clock::now();
    // pack lookup tables
    if (assigner_instance.circuits[0].get_reserved_tables().size() > 0) {
        std::vector <std::size_t> lookup_columns_indices;
        lookup_columns_indices.resize(LookupConstantColumns);
        // fill ComponentConstantColumns, ComponentConstantColumns + 1, ...
        std::iota(lookup_columns_indices.begin(), lookup_columns_indices.end(), ComponentConstantColumns);
        // check if lookup selectors were not used
        auto max_used_selector_idx = assigner_instance.assignments[0].selectors_amount() - 1;
        while (max_used_selector_idx > 0) {
            max_used_selector_idx--;
            if (assigner_instance.assignments[0].selector(max_used_selector_idx).size() > 0) {
                break;
            }
        }

        ASSERT(max_used_selector_idx < ComponentSelectorColumns);

        auto usable_rows_amount = zk::snark::pack_lookup_tables_horizontal(
                assigner_instance.circuits[0].get_reserved_indices(),
                assigner_instance.circuits[0].get_reserved_tables(),
                assigner_instance.circuits[0].get(),
                assigner_instance.assignments[0].get(),
                lookup_columns_indices,
                max_used_selector_idx + 1,
                0,
                max_lookup_rows
        );
    }
    auto pack_lookup_duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - pack_lookup_start);
    BOOST_LOG_TRIVIAL(debug) << "pack_lookup_duration: " << pack_lookup_duration.count() << "ms";
    }

    auto overall_table_printing_start = std::chrono::high_resolution_clock::now();
    constexpr std::uint32_t invalid_target_prover = std::numeric_limits<std::uint32_t>::max();
    // print assignment tables and circuits
    ASSERT_MSG(gen_mode.has_fast_tbl() || (assigner_instance.assignments.size() == assigner_instance.circuits.size()), "Missmatch assignments circuits size");
    if (assigner_instance.assignments.size() == 1 && (target_prover == 0 || target_prover == invalid_target_prover)) {
            auto single_table_print_start = std::chrono::high_resolution_clock::now();
        // print assignment table
        if (gen_mode.has_slow_tbl()) {

            std::array<std::uint32_t, 7> asdf = extract_table_parameters_from_table(
                assigner_instance.assignments[0],
                print_table_kind::SINGLE_PROVER
            );

            print_assignment_table_dynamic<nil::marshalling::option::big_endian, ArithmetizationType, BlueprintFieldType>(
                asdf[6],
                assigner_instance.assignments[0],
                print_table_kind::SINGLE_PROVER,
                assignment_table_file_name,
                0
            );
            auto single_table_print_duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - single_table_print_start);
            BOOST_LOG_TRIVIAL(debug) << "single_table_print_duration: " << single_table_print_duration.count() << "ms";
        }
        if (gen_mode.has_fast_tbl()) {
            auto fast_tbl_print_start = std::chrono::high_resolution_clock::now();

            std::uint32_t padded_rows_amount = padded_rows_from_table_header<nil::marshalling::option::big_endian, ArithmetizationType, BlueprintFieldType>(
                assigner_instance.assignments[0],
                print_table_kind::SINGLE_PROVER,
                assignment_table_file_name,
                0);

            print_assignment_table_dynamic<nil::marshalling::option::big_endian, ArithmetizationType, BlueprintFieldType>(
                padded_rows_amount,
                assigner_instance.assignments[0],
                print_table_kind::SINGLE_PROVER,
                assignment_table_file_name,
                0);

            auto fast_tbl_print_duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - fast_tbl_print_start);
            BOOST_LOG_TRIVIAL(debug) << "fast_tbl_print_duration: " << fast_tbl_print_duration.count() << "ms";
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
                assigner_instance.circuits[0], assigner_instance.assignments[0], false, 0, ocircuit);
            ocircuit.close();

            std::array<std::uint32_t, 7> asdf = extract_table_parameters_from_table(
                assigner_instance.assignments[0],
                print_table_kind::SINGLE_PROVER
            );

            print_assignment_table_static<nil::marshalling::option::big_endian, ArithmetizationType, BlueprintFieldType>(
                asdf,
                assigner_instance.assignments[0],
                print_table_kind::SINGLE_PROVER,
                ComponentConstantColumns,
                ComponentSelectorColumns,
                assignment_table_file_name,
                0
            );



        }
    } else if (assigner_instance.assignments.size() > 1 &&
              (target_prover < assigner_instance.assignments.size() || target_prover == invalid_target_prover)) {
        std::uint32_t start_idx = (target_prover == invalid_target_prover) ? 0 : target_prover;
        std::uint32_t end_idx = (target_prover == invalid_target_prover) ? assigner_instance.assignments.size() : target_prover + 1;

        std::vector<std::future<void>> futures;

        for (std::uint32_t idx = start_idx; idx < end_idx; idx++) {
            // print assignment table
            if (gen_mode.has_slow_tbl()) {

                auto future = std::async(
                    std::launch::async,
                    assignment_table_printer<ArithmetizationType, BlueprintFieldType>,
                    assignment_table_file_name,
                    idx,
                    std::ref(assigner_instance),
                    std::ref(ComponentConstantColumns),
                    std::ref(ComponentSelectorColumns)
                );

                futures.push_back(std::move(future));
            }
            if (gen_mode.has_fast_tbl()) {
                std::uint32_t padded_rows_amount = padded_rows_from_table_header<nil::marshalling::option::big_endian, ArithmetizationType, BlueprintFieldType>(
                    assigner_instance.assignments[idx],
                    print_table_kind::MULTI_PROVER,
                    assignment_table_file_name,
                    idx);

                print_assignment_table_dynamic<nil::marshalling::option::big_endian, ArithmetizationType, BlueprintFieldType>(
                    padded_rows_amount,
                    assigner_instance.assignments[idx],
                    print_table_kind::MULTI_PROVER,
                    assignment_table_file_name,
                    idx);
            }

        }

        for (std::uint32_t idx = start_idx; idx < end_idx; idx++) {
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
                    assigner_instance.circuits[idx], assigner_instance.assignments[idx], true, idx, ocircuit);

                ocircuit.close();

                std::array<std::uint32_t, 7> asdf = extract_table_parameters_from_table(
                    assigner_instance.assignments[idx],
                    print_table_kind::MULTI_PROVER
                );

                print_assignment_table_static<nil::marshalling::option::big_endian, ArithmetizationType, BlueprintFieldType>(
                    asdf,
                    assigner_instance.assignments[idx],
                    print_table_kind::MULTI_PROVER,
                    ComponentConstantColumns,
                    ComponentSelectorColumns,
                    assignment_table_file_name,
                    idx
                );
            }
        }

        for (auto& future : futures) {
            try {
                future.get();
            } catch (const std::exception& e) {
                std::cerr << "Exception from thread: " << e.what() << std::endl;
                return 1;
            }
        }

    } else {
        std::cout << "No data for print: target prover " << target_prover << ", actual number of provers "
                  << assigner_instance.assignments.size() << std::endl;
        return 1;
    }
    auto overall_table_printing_duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - overall_table_printing_start);
    BOOST_LOG_TRIVIAL(debug) << "overall_table_printing_duration: " << overall_table_printing_duration.count() << "ms";

    auto check_validity_start = std::chrono::high_resolution_clock::now();
    if (check_validity && gen_mode.has_assignments()) {
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
    auto check_validity_duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - check_validity_start);
    BOOST_LOG_TRIVIAL(debug) << "check_validity_duration: " << check_validity_duration.count() << "ms";
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
            ("table-pieces,j", boost::program_options::value<std::string>(), "Table pieces json file")
            ("input-column", boost::program_options::value<std::string>(), "Output file for public input column")
            ("elliptic-curve-type,e", boost::program_options::value<std::string>(), "Native elliptic curve type (pallas, vesta, ed25519, bls12381)")
            ("stack-size,s", boost::program_options::value<long>(), "Stack size in bytes")
            ("check", "Check satisfiability of the generated circuit")
            ("log-level,l", boost::program_options::value<std::string>(), "Log level (trace, debug, info, warning, error, fatal)")
            ("print_circuit_output", "deprecated, use \"-f\" instead")
            ("print-circuit-output-format,f", boost::program_options::value<std::string>(), "print output of the circuit (dec, hex)")
            ("policy", boost::program_options::value<std::string>(), "Policy for creating circuits. Possible values: default")
            ("generate-type", boost::program_options::value<std::string>(), "Define generated output. Possible values: circuit, assignment, assignment-fast, circuit-assignment, public-input-column, size_estimation(does not generate anything, just evaluates circuit size). Default value is circuit-assignment")
            ("max-num-provers", boost::program_options::value<int>(), "Maximum number of provers. Possible values >= 1")
            ("max-lookup-rows", boost::program_options::value<int>(), "Maximum number of lookup rows")
            ("target-prover", boost::program_options::value<int>(), "Assignment table and circuit will be generated only for defined prover. Possible values [0, max-num-provers)")
            ("column-sizes", boost::program_options::value<std::vector<std::size_t>>()->multitoken(), "Column sizes, 6 values: WitnessColumns, PublicInputColumns, ComponentConstantColumns, LookupConstantColumns, ComponentSelectorColumns, LookupSelectorColumns")
            ;
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
    std::string table_pieces_file_name;
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

    nil::blueprint::generation_mode gen_mode = nil::blueprint::generation_mode::slow_tbl() | nil::blueprint::generation_mode::circuit();
    if (vm.count("generate-type")) {
        const auto generate_type = vm["generate-type"].as<std::string>();
        if (generate_type == "circuit") {
            gen_mode = nil::blueprint::generation_mode::circuit();
        } else if (generate_type == "assignment") {
            gen_mode = nil::blueprint::generation_mode::slow_tbl();
        } else if (generate_type == "assignment-fast") {
            gen_mode = nil::blueprint::generation_mode::fast_tbl();
        } else if (generate_type == "size_estimation") {
            gen_mode = nil::blueprint::generation_mode::size_estimation() | nil::blueprint::generation_mode::circuit();
        } else if (generate_type == "public-input-column") {
            gen_mode = nil::blueprint::generation_mode::public_input_column();
        } else if (generate_type != "circuit-assignment") {
            std::cerr << "Invalid command line argument - generate-type. " << generate_type << " is wrong value." << std::endl;
            std::cout << options_desc << std::endl;
            return 1;
        }
    }
    gen_mode = gen_mode | nil::blueprint::generation_mode::assignments(); // for now in any case we generate assignment table to determine used_rows

    if (!vm.count("public-input") && !vm.count("private-input") && !gen_mode.has_size_estimation()) {
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

    if (gen_mode.has_circuit() || gen_mode.has_fast_tbl() || gen_mode.has_slow_tbl()) {
        if (vm.count("assignment-table")) {
            assignment_table_file_name = vm["assignment-table"].as<std::string>();
        } else {
            if (!gen_mode.has_size_estimation()) {
                std::cerr << "Invalid command line argument - assignment table file name is not specified" << std::endl;
                std::cout << options_desc << std::endl;
                return 1;
            }
        }
    }

    if (gen_mode.has_circuit()) {
        if (vm.count("circuit")) {
            circuit_file_name = vm["circuit"].as<std::string>();
        } else {
            if (!gen_mode.has_size_estimation()) {
                std::cerr << "Invalid command line argument - circuit file name is not specified" << std::endl;
                std::cout << options_desc << std::endl;
                return 1;
            }
        }
    }

    if (vm.count("table-pieces")) {
        table_pieces_file_name = vm["table-pieces"].as<std::string>();
    }
    // else {
    //     std::cerr << "Invalid command line argument - table-pieces file name is not specified" << std::endl;
    //     std::cout << options_desc << std::endl;
    //     return 1;
    // }

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
            std::cerr << "Invalid command line argument --max-num-provers. " << max_num_provers << " is wrong value." << std::endl;
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

    const std::size_t column_sizes_size = 6;
    std::array<std::size_t, column_sizes_size> column_sizes = {
        ParametersPolicy::WitnessColumns,
        ParametersPolicy::PublicInputColumns,
        ParametersPolicy::ComponentConstantColumns,
        ParametersPolicy::LookupConstantColumns,
        ParametersPolicy::ComponentSelectorColumns,
        ParametersPolicy::LookupSelectorColumns
    };

    if (vm.count("column-sizes")) {
        std::vector<std::size_t> column_sizes_vector = vm["column-sizes"].as<std::vector<std::size_t>>();
        ASSERT_MSG(column_sizes_vector.size() == column_sizes_size, ("Column sizes amount must be exactly " + std::to_string(column_sizes_size) + ", but it is " + std::to_string(column_sizes_vector.size())).c_str());
        for (std::size_t i = 0; i < column_sizes_size; i++) {
            column_sizes[i] = column_sizes_vector[i];
        }
    }

    switch (curve_options[elliptic_curve]) {
        case 0: {
            return curve_dependent_main<typename algebra::curves::pallas::base_field_type>(
                                                                          bytecode_file_name,
                                                                          public_input_file_name,
                                                                          private_input_file_name,
                                                                          assignment_table_file_name,
                                                                          circuit_file_name,
                                                                          table_pieces_file_name,
                                                                          processed_public_input_file_name,
                                                                          stack_size,
                                                                          vm.count("check"),
                                                                          log_options[log_level],
                                                                          policy,
                                                                          gen_mode,
                                                                          max_num_provers,
                                                                          max_lookup_rows,
                                                                          target_prover,
                                                                          circuit_output_print_format,
                                                                          column_sizes
                                                                          );
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
                                                                          table_pieces_file_name,
                                                                          processed_public_input_file_name,
                                                                          stack_size,
                                                                          vm.count("check"),
                                                                          log_options[log_level],
                                                                          policy,
                                                                          gen_mode,
                                                                          max_num_provers,
                                                                          max_lookup_rows,
                                                                          target_prover,
                                                                          circuit_output_print_format,
                                                                          column_sizes
                                                                          );
            break;
        }
    };

    return 0;
}
