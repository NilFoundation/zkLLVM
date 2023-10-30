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
#include <nil/crypto3/marshalling/zk/types/plonk/assignment_table.hpp>

#include <nil/blueprint/parser.hpp>
#include <nil/blueprint/asserts.hpp>
#include <nil/blueprint/utils/satisfiability_check.hpp>

#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Signals.h>

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
        nil::crypto3::marshalling::types::fill_plonk_constraint_system<Endianness, ConstraintSystemType>(circuit);

    std::vector<std::uint8_t> cv;
    cv.resize(filled_val.length(), 0x00);
    auto write_iter = cv.begin();
    nil::marshalling::status_type status = filled_val.write(write_iter, cv.size());
    print_hex_byteblob(out, cv.cbegin(), cv.cend(), false);
}

template<typename Endianness, typename AssignmentTableType>
void print_assignment_table(std::size_t usable_rows, const AssignmentTableType &table, std::ostream &out = std::cout) {
    using TTypeBase = nil::marshalling::field_type<Endianness>;
    auto filled_val =
        nil::crypto3::marshalling::types::fill_assignment_table<Endianness, AssignmentTableType>(usable_rows, table);

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
                          bool verbose,
                          const std::string &policy) {
    using BlueprintFieldType = typename CurveType::base_field_type;
    constexpr std::size_t WitnessColumns = 15;
    constexpr std::size_t PublicInputColumns = 1;
    constexpr std::size_t ConstantColumns = 5;
    constexpr std::size_t SelectorColumns = 70;

    using ArithmetizationParams =
        zk::snark::plonk_arithmetization_params<WitnessColumns, PublicInputColumns, ConstantColumns, SelectorColumns>;
    using ConstraintSystemType = zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;

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
                                                                                                          verbose, policy);

    std::unique_ptr<llvm::Module> module = parser_instance.parseIRFile(bytecode_file_name.c_str());
    if (module == nullptr) {
        return 1;
    }

    if (!parser_instance.evaluate(*module, input_json_value.as_array())) {
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

    using AssignmentTableType = zk::snark::plonk_table<BlueprintFieldType, ArithmetizationParams, zk::snark::plonk_column<BlueprintFieldType>>;
    print_assignment_table<
        nil::marshalling::option::big_endian,
        AssignmentTableType
    >(desc.usable_rows_amount, parser_instance.assignmnt, otable);

//    nil::blueprint::profiling_assignment_table(parser_instance.assignmnt, desc.usable_rows_amount, otable);
    otable.close();

    std::ofstream ocircuit;
    ocircuit.open(circuit_file_name);
    if (!ocircuit) {
        std::cout << "Something wrong with output " << circuit_file_name << std::endl;
        return 1;
    }
    print_circuit<nil::marshalling::option::big_endian, ConstraintSystemType>(parser_instance.bp, ocircuit);
    ocircuit.close();

    if (check_validity){
        bool is_satisfied = nil::blueprint::is_satisfied(parser_instance.bp, parser_instance.assignmnt);
        ASSERT_MSG(is_satisfied, "The circuit is not satisfied" );
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
            ("verbose", "Print detailed log")
            ("print_circuit_output", "print output of the circuit")
            ("policy", boost::program_options::value<std::string>(), "Policy for creating circuits. Possible values: default");
    // clang-format on

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(options_desc).run(),
                                  vm);
    boost::program_options::notify(vm);

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
                                                                          vm.count("verbose"),
                                                                          policy);
            }
            else {
                return curve_dependent_main<typename algebra::curves::pallas, false>(
                                                                          bytecode_file_name,
                                                                          public_input_file_name,
                                                                          assignment_table_file_name,
                                                                          circuit_file_name,
                                                                          stack_size,
                                                                          vm.count("check"),
                                                                          vm.count("verbose"),
                                                                          policy);
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
