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

#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#define BOOST_FILESYSTEM_NO_DEPRECATED
#endif
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#define BOOST_SYSTEM_NO_DEPRECATED
#endif

#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <boost/json/src.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

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

#include <nil/blueprint/parser.hpp>
#include <nil/blueprint/utils/table_profiling.hpp>
#include <nil/blueprint/utils/satisfiability_check.hpp>

#include <iostream>
#include <vector>
#include <random>

#include <nil/crypto3/marshalling/zk/types/placeholder/proof.hpp>

#include <nil/crypto3/zk/snark/systems/plonk/placeholder/preprocessor.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/prover.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/verifier.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/params.hpp>

#include <nil/crypto3/math/polynomial/polynomial.hpp>
#include <nil/crypto3/math/algorithms/calculate_domain_set.hpp>

// #include <nil/blueprint/transpiler/minimized_profiling_plonk_circuit.hpp>

bool read_buffer_from_file(std::ifstream &ifile, std::vector<std::uint8_t> &v) {
    char c;
    char c1;
    uint8_t b;

    ifile >> c;
    if (c != '0')
        return false;
    ifile >> c;
    if (c != 'x')
        return false;
    while (ifile) {
        std::string str = "";
        ifile >> c >> c1;
        if (!isxdigit(c) || !isxdigit(c1))
            return false;
        str += c;
        str += c1;
        b = stoi(str, 0, 0x10);
        v.push_back(b);
    }
    return true;
}

template<typename BlueprintFieldType, typename ArithmetizationParams, typename ColumnType>
std::tuple<std::size_t, std::size_t,
           nil::crypto3::zk::snark::plonk_table<BlueprintFieldType, ArithmetizationParams, ColumnType>>
    load_assignment_table(std::istream &istr) {
    using PrivateTableType =
        nil::crypto3::zk::snark::plonk_private_table<BlueprintFieldType, ArithmetizationParams, ColumnType>;
    using PublicTableType =
        nil::crypto3::zk::snark::plonk_public_table<BlueprintFieldType, ArithmetizationParams, ColumnType>;
    using TableAssignmentType =
        nil::crypto3::zk::snark::plonk_table<BlueprintFieldType, ArithmetizationParams, ColumnType>;
    std::size_t usable_rows;
    std::size_t rows_amount;

    typename PrivateTableType::witnesses_container_type witness;
    typename PublicTableType::public_input_container_type public_input;
    typename PublicTableType::constant_container_type constant;
    typename PublicTableType::selector_container_type selector;

    istr >> usable_rows;
    istr >> rows_amount;

    for (size_t i = 0; i < witness.size(); i++) {    // witnesses.size() == ArithmetizationParams.WitnessColumns
        ColumnType column;
        typename BlueprintFieldType::integral_type num;
        for (size_t j = 0; j < rows_amount; j++) {
            istr >> num;
            column.push_back(typename BlueprintFieldType::value_type(num));
        }
        witness[i] = column;
    }

    for (size_t i = 0; i < public_input.size(); i++) {    // witnesses.size() == ArithmetizationParams.WitnessColumns
        ColumnType column;
        typename BlueprintFieldType::integral_type num;
        for (size_t j = 0; j < rows_amount; j++) {
            istr >> num;
            column.push_back(typename BlueprintFieldType::value_type(num));
        }
        public_input[i] = column;
    }

    for (size_t i = 0; i < constant.size(); i++) {    // witnesses.size() == ArithmetizationParams.WitnessColumns
        ColumnType column;
        typename BlueprintFieldType::integral_type num;
        for (size_t j = 0; j < rows_amount; j++) {
            istr >> num;
            column.push_back(typename BlueprintFieldType::value_type(num));
        }
        constant[i] = column;
    }
    for (size_t i = 0; i < selector.size(); i++) {    // witnesses.size() == ArithmetizationParams.WitnessColumns
        ColumnType column;
        typename BlueprintFieldType::integral_type num;
        for (size_t j = 0; j < rows_amount; j++) {
            istr >> num;
            column.push_back(typename BlueprintFieldType::value_type(num));
        }
        selector[i] = column;
    }
    return std::make_tuple(
        usable_rows, rows_amount,
        TableAssignmentType(PrivateTableType(witness), PublicTableType(public_input, constant, selector)));
}

inline std::vector<std::size_t> generate_random_step_list(const std::size_t r, const int max_step) {
    using dist_type = std::uniform_int_distribution<int>;
    static std::random_device random_engine;

    std::vector<std::size_t> step_list;
    std::size_t steps_sum = 0;
    while (steps_sum != r) {
        if (r - steps_sum <= max_step) {
            while (r - steps_sum != 1) {
                step_list.emplace_back(r - steps_sum - 1);
                steps_sum += step_list.back();
            }
            step_list.emplace_back(1);
            steps_sum += step_list.back();
        } else {
            step_list.emplace_back(dist_type(1, max_step)(random_engine));
            steps_sum += step_list.back();
        }
    }
    return step_list;
}

template<typename FRIScheme, typename FieldType>
typename FRIScheme::params_type create_fri_params(std::size_t degree_log, const int max_step = 1) {
    typename FRIScheme::params_type params;
    nil::crypto3::math::polynomial<typename FieldType::value_type> q = {0, 0, 1};

    constexpr std::size_t expand_factor = 0;
    std::size_t r = degree_log - 1;

    std::vector<std::shared_ptr<nil::crypto3::math::evaluation_domain<FieldType>>> domain_set =
        nil::crypto3::math::calculate_domain_set<FieldType>(degree_log + expand_factor, r);

    params.r = r;
    params.D = domain_set;
    params.max_degree = (1 << degree_log) - 1;
    params.step_list = generate_random_step_list(r, max_step);

    return params;
}

using namespace nil;
using namespace nil::crypto3;

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

template<typename Endianness, typename ConstraintSystemType>
void print_circuit(const ConstraintSystemType &circuit, std::ostream &out = std::cout) {

    using TTypeBase = nil::marshalling::field_type<Endianness>;
    auto filled_val =
        nil::crypto3::marshalling::types::fill_plonk_constraint_system<ConstraintSystemType, Endianness>(circuit);

    std::vector<std::uint8_t> cv;
    cv.resize(filled_val.length(), 0x00);
    auto write_iter = cv.begin();
    nil::marshalling::status_type status = filled_val.write(write_iter, cv.size());
    print_hex_byteblob(out, cv.cbegin(), cv.cend(), false);
}

template<typename CurveType>
bool curve_dependent_main(std::string bytecode_file_name,
                          std::string public_input_file_name,
                          std::string assignment_table_file_name,
                          std::string circuit_file_name) {
    using BlueprintFieldType = typename CurveType::base_field_type;
    constexpr std::size_t WitnessColumns = 15;
    constexpr std::size_t PublicInputColumns = 5;
    constexpr std::size_t ConstantColumns = 5;
    constexpr std::size_t SelectorColumns = 50;

    using ArithmetizationParams =
        zk::snark::plonk_arithmetization_params<WitnessColumns, PublicInputColumns, ConstantColumns, SelectorColumns>;
    using ConstraintSystemType = zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;

    std::vector<typename BlueprintFieldType::value_type> public_input;
    auto fptr = std::fopen(public_input_file_name.c_str(), "r");
    if (fptr == NULL) {
        std::cerr << "Could not open the file - '" << public_input_file_name << "'" << std::endl;
        return EXIT_FAILURE;
    }

    while (!std::feof(fptr)) {
        char input_string[256];
        fscanf(fptr, "%s\n", input_string);
        typename CurveType::base_field_type::extended_integral_type number(input_string);
        assert(number < BlueprintFieldType::modulus && "input does not fit into BlueprintFieldType");
        public_input.push_back(number);
    }
    nil::blueprint::parser<BlueprintFieldType, ArithmetizationParams> parser_instance;

    std::unique_ptr<llvm::Module> module = parser_instance.parseIRFile(bytecode_file_name.c_str());
    if (module == nullptr) {
        return 1;
    }

    if (!parser_instance.evaluate(*module, public_input)) {
        return 1;
    }

    zk::snark::plonk_table_description<BlueprintFieldType, ArithmetizationParams> desc;
    desc.usable_rows_amount = parser_instance.assignmnt.rows_amount();
    desc.rows_amount = zk::snark::basic_padding(parser_instance.assignmnt);

    std::ofstream otable;
    otable.open(assignment_table_file_name);
    if (!otable) {
        std::cout << "Something wrong with output " << assignment_table_file_name << std::endl;
        return 1;
    }
    nil::blueprint::profiling_assignment_table(parser_instance.assignmnt, desc.usable_rows_amount, otable);
    otable.close();

    std::ofstream ocircuit;
    ocircuit.open(circuit_file_name);
    if (!ocircuit) {
        std::cout << "Something wrong with output " << circuit_file_name << std::endl;
        return 1;
    }
    print_circuit<nil::marshalling::option::big_endian, ConstraintSystemType>(parser_instance.bp, ocircuit);
    ocircuit.close();

    //////////////////////////

    std::ifstream ifile;
    std::ofstream ofile;
    ifile.open(circuit_file_name);
    if (!ifile.is_open()) {
        std::cout << "Cannot find input file " << circuit_file_name << std::endl;
        return 1;
    }
    std::vector<std::uint8_t> v;
    if (!read_buffer_from_file(ifile, v)) {
        std::cout << "Cannot parse input file " << circuit_file_name << std::endl;
        return 1;
    }
    ifile.close();




    using ConstraintSystemType =
        nil::crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;
    using TableDescriptionType =
        nil::crypto3::zk::snark::plonk_table_description<BlueprintFieldType, ArithmetizationParams>;
    using Endianness = nil::marshalling::option::big_endian;
    using TTypeBase = nil::marshalling::field_type<Endianness>;
    using value_marshalling_type =
        nil::crypto3::marshalling::types::plonk_constraint_system<TTypeBase, ConstraintSystemType>;
    using ColumnsRotationsType = std::array<std::vector<int>, ArithmetizationParams::total_columns>;
    // using ProfilingType = nil::crypto3::minimized_profiling_plonk_circuit<BlueprintFieldType, ArithmetizationParams>;

    value_marshalling_type marshalled_data;
    TableDescriptionType table_description;
    auto read_iter = v.begin();
    auto status = marshalled_data.read(read_iter, v.size());
    auto constraint_system =
        nil::crypto3::marshalling::types::make_plonk_constraint_system<ConstraintSystemType, Endianness>(
            marshalled_data);

    using ColumnType = nil::crypto3::zk::snark::plonk_column<BlueprintFieldType>;
    using TableAssignmentType =
        nil::crypto3::zk::snark::plonk_table<BlueprintFieldType, ArithmetizationParams, ColumnType>;

    std::ifstream iassignment;
    iassignment.open(assignment_table_file_name);
    if (!iassignment) {
        std::cout << "Cannot open " << assignment_table_file_name << std::endl;
        return 1;
    }
    TableAssignmentType assignment_table;
    std::tie(table_description.usable_rows_amount, table_description.rows_amount, assignment_table) =
        load_assignment_table<BlueprintFieldType, ArithmetizationParams, ColumnType>(iassignment);
    // auto columns_rotations = ProfilingType::columns_rotations(constraint_system, table_description);

    const std::size_t Lambda = 2;
    using Hash = nil::crypto3::hashes::keccak_1600<256>;
    using placeholder_params =
        nil::crypto3::zk::snark::placeholder_params<BlueprintFieldType, ArithmetizationParams, Hash, Hash, Lambda>;
    using types = nil::crypto3::zk::snark::detail::placeholder_policy<BlueprintFieldType, placeholder_params>;

    using FRIScheme =
        typename nil::crypto3::zk::commitments::fri<BlueprintFieldType, typename placeholder_params::merkle_hash_type,
                                                    typename placeholder_params::transcript_hash_type, 2, 1>;
    using FRIParamsType = typename FRIScheme::params_type;

    std::size_t table_rows_log = std::ceil(std::log2(table_description.rows_amount));
    auto fri_params = create_fri_params<FRIScheme, BlueprintFieldType>(table_rows_log);
    std::size_t permutation_size =
        table_description.witness_columns + table_description.public_input_columns + table_description.constant_columns;


    typename nil::crypto3::zk::snark::placeholder_public_preprocessor<
        BlueprintFieldType, placeholder_params>::preprocessed_data_type public_preprocessed_data =
        nil::crypto3::zk::snark::placeholder_public_preprocessor<BlueprintFieldType, placeholder_params>::process(
            constraint_system, assignment_table.public_table(), table_description, fri_params, permutation_size);
        typename nil::crypto3::zk::snark::placeholder_private_preprocessor<
            BlueprintFieldType, placeholder_params>::preprocessed_data_type private_preprocessed_data =
            nil::crypto3::zk::snark::placeholder_private_preprocessor<BlueprintFieldType, placeholder_params>::process(
                constraint_system, assignment_table.private_table(), table_description, fri_params);

        using ProofType = nil::crypto3::zk::snark::placeholder_proof<BlueprintFieldType, placeholder_params>;
        ProofType proof = nil::crypto3::zk::snark::placeholder_prover<BlueprintFieldType, placeholder_params>::process(
            public_preprocessed_data, private_preprocessed_data, table_description, constraint_system, assignment_table, fri_params);

        bool verifier_res = nil::crypto3::zk::snark::placeholder_verifier<BlueprintFieldType, placeholder_params>::process(
            public_preprocessed_data, proof, constraint_system, fri_params);
            
        if( verifier_res ){
            // auto filled_placeholder_proof = nil::crypto3::marshalling::types::fill_placeholder_proof<Endianness, ProofType>(proof);
            // proof_print<Endianness, ProofType>(proof, ofolder_path+"/proof.bin");
            std::cout << "Proof is verified" << std::endl;
            return 0;
        } else {
            std::cout << "Proof is not verified" << std::endl;
            return 1;
        }

        iassignment.close();

    //////////////////

    return !nil::blueprint::is_satisfied(parser_instance.bp, parser_instance.assignmnt);
}

int main(int argc, char *argv[]) {

    boost::program_options::options_description options_desc("zkLLVM assigner");

    // clang-format off
    options_desc.add_options()("help,h", "Display help message")
            ("version,v", "Display version")
            ("bytecode,b", boost::program_options::value<std::string>(), "Bytecode input file")
            ("public-input,i", boost::program_options::value<std::string>(), "Public input file")
            ("assignment-table,t", boost::program_options::value<std::string>(), "Assignment table output file")
            ("circuit,c", boost::program_options::value<std::string>(), "Circuit output file")
            ("elliptic-curve-type,e", boost::program_options::value<int>(), "Native elliptic curve type (0=pallas, 1=vesta, 2=ed25519, 3=bls12381)");
    // clang-format on

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(options_desc).run(),
                                  vm);
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        std::cout << options_desc << std::endl;
        return 0;
    }

    std::string bytecode_file_name;
    std::string public_input_file_name;
    std::string assignment_table_file_name;
    std::string circuit_file_name;
    int elliptic_curve;

    if (vm.count("bytecode")) {
        bytecode_file_name = vm["bytecode"].as<std::string>();
    }

    if (vm.count("public-input")) {
        public_input_file_name = vm["public-input"].as<std::string>();
    }

    if (vm.count("assignment-table")) {
        assignment_table_file_name = vm["assignment-table"].as<std::string>();
    }

    if (vm.count("circuit")) {
        circuit_file_name = vm["circuit"].as<std::string>();
    }

    if (vm.count("elliptic-curve-type")) {
        elliptic_curve = vm["elliptic-curve-type"].as<int>();
    }

    switch (elliptic_curve) {
        case 0: {
            return curve_dependent_main<typename algebra::curves::pallas>(bytecode_file_name, public_input_file_name, assignment_table_file_name, circuit_file_name);
            break;
        }
        case 1: {
            std::cerr << "command line argument -e 1: vesta curve is not supported yet" << std::endl;
            assert(1==0 && "vesta curve is not supported yet");
            break;
        }
        case 2: {
            std::cerr << "command line argument -e 2: ed25519 curve is not supported yet" << std::endl;
            assert(1==0 && "ed25519 curve is not supported yet");
            break;
        }
        case 3: {
            std::cerr << "command line argument -e 3: bls12381 curve is not supported yet" << std::endl;
            assert(1==0 && "bls12381 curve is not supported yet");
            break;
        }
        default:
            std::cerr << "invalid command line argument -e" << std::endl;
            assert(1 == 0 && "invalid curve type");
    };
}
