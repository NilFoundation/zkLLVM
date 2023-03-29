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

#include <llvm/Support/CommandLine.h>

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

    const char *llvm_arguments[2] = {"", "-opaque-pointers=0"};
    llvm::cl::ParseCommandLineOptions(2, llvm_arguments);

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
            ("elliptic-curve-type,e", boost::program_options::value<std::string>(), "Native elliptic curve type (pallas, vesta, ed25519, bls12-381)");
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
    std::string elliptic_curve;

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

    switch (curve_options[elliptic_curve]) {
        case 0: {
            return curve_dependent_main<typename algebra::curves::pallas>(bytecode_file_name, public_input_file_name, assignment_table_file_name, circuit_file_name);
            break;
        }
        case 1: {
            std::cerr << "command line argument -e vesta is not supported yet" << std::endl;
            assert(1==0 && "vesta curve based circuits are not supported yet");
            break;
        }
        case 2: {
            std::cerr << "command line argument -e ed25519 is not supported yet" << std::endl;
            assert(1==0 && "ed25519 curve based circuits are not supported yet");
            break;
        }
        case 3: {
            std::cerr << "command line argument -e bls12-381 is not supported yet" << std::endl;
            assert(1==0 && "bls12-381 curve based circuits are not supported yet");
            break;
        }
    };

    return 0;
}
