#include <iostream>
#include <fstream>
#include <vector>
#include <random>

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
#include <boost/test/unit_test.hpp>

#include <nil/crypto3/algebra/curves/pallas.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/pallas.hpp>

#include <nil/crypto3/zk/snark/arithmetization/plonk/params.hpp>
#include <nil/crypto3/zk/snark/arithmetization/plonk/constraint_system.hpp>
#include <nil/crypto3/zk/math/expression.hpp>
#include <nil/crypto3/zk/math/expression_visitors.hpp>
#include <nil/crypto3/zk/math/expression_evaluator.hpp>

#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>
#include <nil/crypto3/hash/md5.hpp>
#include <nil/crypto3/hash/keccak.hpp>
#include <nil/crypto3/hash/poseidon.hpp>

#include <nil/marshalling/status_type.hpp>
#include <nil/marshalling/field_type.hpp>
#include <nil/marshalling/endianness.hpp>
#include <nil/crypto3/marshalling/zk/types/plonk/constraint_system.hpp>
#include <nil/crypto3/marshalling/zk/types/plonk/assignment_table.hpp>
#include <nil/crypto3/marshalling/zk/types/placeholder/common_data.hpp>

#include <nil/crypto3/zk/snark/systems/plonk/placeholder/preprocessor.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/prover.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/verifier.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/params.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/detail/profiling.hpp>

#include <nil/crypto3/math/polynomial/polynomial.hpp>
#include <nil/crypto3/math/algorithms/calculate_domain_set.hpp>

#include <nil/blueprint/asserts.hpp>
#include <nil/blueprint/transpiler/minimized_profiling_plonk_circuit.hpp>
#include <nil/blueprint/transpiler/public_input.hpp>
#include <nil/blueprint/transpiler/recursive_verifier_generator.hpp>

#include "../../table_packing.hpp"

template<typename StreamType>
std::optional<StreamType> open_file(const std::string& path, std::ios_base::openmode mode) {
    StreamType file(path, mode);
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << path;
        return std::nullopt;
    }

    return file;
}

std::optional<std::vector<std::uint8_t>> read_file_to_vector(const std::string& path) {

    auto file = open_file<std::ifstream>(path, std::ios_base::in | std::ios::binary | std::ios::ate);
    if (!file.has_value()) {
        return std::nullopt;
    }

    std::ifstream& stream = file.value();
    std::streamsize fsize = stream.tellg();
    stream.seekg(0, std::ios::beg);
    std::vector<std::uint8_t> v(static_cast<size_t>(fsize));

    stream.read(reinterpret_cast<char*>(v.data()), fsize);

    if (stream.fail()) {
        std::cerr << "Error occurred during reading file " << path << std::endl;
        return std::nullopt;
    }

    return v;
}

template<typename MarshallingType>
std::optional<MarshallingType> decode_marshalling_from_file(
    const boost::filesystem::path& path
    //bool hex = false  // Used only for placeholder proof
) {
    const auto v =  read_file_to_vector(path.c_str());
    if (!v.has_value()) {
        return std::nullopt;
    }

    MarshallingType marshalled_data;
    auto read_iter = v->begin();
    auto status = marshalled_data.read(read_iter, v->size());
    if (status != nil::marshalling::status_type::success) {
        std::cerr << "Marshalled structure decoding failed" << std::endl;
        return std::nullopt;
    }
    return marshalled_data;
}


template<typename BlueprintFieldType>
struct ParametersPolicy {
    constexpr static const std::size_t WitnessColumns = WITNESS_COLUMNS;
    constexpr static const std::size_t PublicInputColumns = PUBLIC_INPUT_COLUMNS;
    constexpr static const std::size_t ComponentConstantColumns = COMPONENT_CONSTANT_COLUMNS;
    constexpr static const std::size_t LookupConstantColumns = LOOKUP_CONSTANT_COLUMNS;
    constexpr static const std::size_t ComponentSelectorColumns = COMPONENT_SELECTOR_COLUMNS;
    constexpr static const std::size_t LookupSelectorColumns = LOOKUP_SELECTOR_COLUMNS;
    constexpr static const std::size_t lambda = LAMBDA;
    constexpr static const std::size_t GrindingBits = GRINDING_BITS;
    constexpr static const bool UseGrinding = (GrindingBits != 0);

private:
    using poseidon_policy = nil::crypto3::hashes::detail::mina_poseidon_policy<BlueprintFieldType>;
    using default_hash = nil::crypto3::hashes::poseidon<poseidon_policy>;
public:
    using hash =default_hash;
};

template<typename BlueprintFieldType, typename ConstraintSystemType, typename ColumnsRotationsType>
void print_sol_files(
    zk::snark::plonk_table_description<BlueprintFieldType> desc,
    ConstraintSystemType &constraint_system,
    ColumnsRotationsType &columns_rotations,
    std::string out_folder_path = ".",
    bool optimize_gates = false
) {
    nil::blueprint::minimized_profiling_plonk_circuit<BlueprintFieldType> profiling_instance(desc);
    profiling_instance.process_split(
        nil::blueprint::main_sol_file_template,
        nil::blueprint::gate_sol_file_template,
        constraint_system,
        columns_rotations,
        out_folder_path,
        optimize_gates
    );
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
/*
template<typename Endianness, typename Proof>
void proof_print(Proof &proof, const std::string &output_file) {
    using namespace nil::crypto3::marshalling;

    using TTypeBase = nil::marshalling::field_type<Endianness>;
    using proof_marshalling_type = nil::crypto3::zk::snark::placeholder_proof<TTypeBase, Proof>;
    auto filled_placeholder_proof = types::fill_placeholder_proof<Endianness, Proof>(proof);

    std::vector<std::uint8_t> cv;
    cv.resize(filled_placeholder_proof.length(), 0x00);
    auto write_iter = cv.begin();
    nil::marshalling::status_type status = filled_placeholder_proof.write(write_iter, cv.size());
    std::ofstream out;
    out.open(output_file);
    print_hex_byteblob(out, cv.cbegin(), cv.cend(), false);
}
*/
template<typename BlueprintFieldType, bool multiprover>
int curve_dependent_main(
    boost::program_options::options_description options_desc,
    boost::program_options::variables_map vm
);

int main(int argc, char *argv[]) {

    boost::program_options::options_description options_desc("zkLLVM recursive verifier generator");

    // clang-format off
    options_desc.add_options()("help,h", "Display help message")
            ("mode,m", boost::program_options::value<std::string>(), "Recursive verifier mode (gen-input, gen-verifier).\
            gen-input generates test proof and recursive verifier input.\
            gen-verifier generates main.cpp file for recursive verifier for given circuit.\
             ")
            ("public-input,i", boost::program_options::value<std::string>(), "Public input file")
            ("assignment-table,t", boost::program_options::value<std::string>(), "Assignment table input file")
            ("circuit,c", boost::program_options::value<std::string>(), "Circuit input file")
            ("common-data,d", boost::program_options::value<std::string>(), "Common data file")
            ("output-folder-path,o", boost::program_options::value<std::string>(), "Output folder absolute path.\
            It'll be better to create an empty folder for output")
            ("skip-verification", "Used with gen-test-proof, if set - skips verifiyng the generated proof")
            ("multi-prover", "Pass this flag if input circuit is a part of larger circuit, divided for faster paralel proving")
            ("lambda,l", boost::program_options::value<std::size_t>(), "Number of FRI rounds")
            ("expand-factor", boost::program_options::value<std::size_t>(), "FRI blow-up factor")
            ("max-quotient-chunks", boost::program_options::value<std::size_t>(), "Maximal parts of quotient polynomial")
            ("elliptic-curve-type,e", boost::program_options::value<std::string>(), "Native elliptic curve type (pallas, vesta, ed25519, bls12381)");

    // clang-format on
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(options_desc).run(), vm);
    boost::program_options::notify(vm);

    std::string elliptic_curve;

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
        {"bls12381", 3},
    };

    if (curve_options.find(elliptic_curve) == curve_options.end()) {
        std::cerr << "Invalid command line argument -e (Native elliptic curve type): " << elliptic_curve << std::endl;
        std::cout << options_desc << std::endl;
        return 1;
    }

    switch (curve_options[elliptic_curve]) {
        case 0: {
            using curve_type = nil::crypto3::algebra::curves::pallas;
            using BlueprintFieldType = typename curve_type::base_field_type;
            return (vm.count("multi-prover") > 0) ?
                curve_dependent_main<BlueprintFieldType, true>(options_desc, vm) :
                curve_dependent_main<BlueprintFieldType, false>(options_desc, vm);
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
            UNREACHABLE("bls12381 curve based circuits are not supported yet");
            break;
        }
    };
}

template<typename BlueprintFieldType, bool is_multi_prover>
int curve_dependent_main(
    boost::program_options::options_description options_desc,
    boost::program_options::variables_map vm
) {
    if (vm.count("help")) {
        std::cout << options_desc << std::endl;
        return 0;
    }

    if (vm.count("version")) {
#ifdef RECURSIVE_VERSION
#define xstr(s) str(s)
#define str(s) #s
        std::cout << xstr(RECURSIVE_VERSION) << std::endl;
#else
        std::cout << "undefined" << std::endl;
#endif
        return 0;
    }

    std::string mode;
    std::string assignment_table_file_name;
    std::string common_data_file_name;
    std::string circuit_file_name;
    std::string output_folder_path;
    std::string public_input;

    if (vm.count("mode")) {
        mode = vm["mode"].as<std::string>();
    } else {
        std::cerr << "Invalid mode specified" << std::endl;
        std::cout << options_desc << std::endl;
        return 1;
    }

    if ((mode != "gen-input" && mode != "gen-verifier")) {
        std::cerr << "Invalid mode specified" << std::endl;
        std::cout << options_desc << std::endl;
        return 1;
    }

    if (vm.count("assignment-table")) {
        assignment_table_file_name = vm["assignment-table"].as<std::string>();
    }

    if (vm.count("common-data")) {
        common_data_file_name = vm["common-data"].as<std::string>();
    }

    if (vm.count("circuit")) {
        circuit_file_name = vm["circuit"].as<std::string>();
    } else {
        std::cerr << "Invalid command line argument - circuit file name is not specified" << std::endl;
        std::cout << options_desc << std::endl;
        return 1;
    }

    if (vm.count("output-folder-path")) {
        output_folder_path = vm["output-folder-path"].as<std::string>();
        boost::filesystem::path dir(output_folder_path);
        if(boost::filesystem::create_directory(output_folder_path))
        {
            std::cerr<< "Directory Created: "<<output_folder_path<<std::endl;
        }
    } else {
        std::cerr << "Invalid command line argument - output folder path is not specified" << std::endl;
        std::cout << options_desc << std::endl;
        return 1;
    }

    using parameters_policy = ParametersPolicy<BlueprintFieldType>;
    constexpr std::size_t WitnessColumns = parameters_policy::WitnessColumns;
    constexpr std::size_t PublicInputColumns = is_multi_prover? parameters_policy::PublicInputColumns + 1: parameters_policy::PublicInputColumns;
    constexpr std::size_t ConstantColumns = parameters_policy::ComponentConstantColumns + parameters_policy::LookupConstantColumns;
    constexpr std::size_t SelectorColumns = parameters_policy::ComponentSelectorColumns + parameters_policy::LookupSelectorColumns;

    // Circuit-specific parameter
    using ConstraintSystemType =
        nil::crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType>;
    using TableDescriptionType =
        nil::crypto3::zk::snark::plonk_table_description<BlueprintFieldType>;
    using Endianness = nil::marshalling::option::big_endian;
    using TTypeBase = nil::marshalling::field_type<Endianness>;
    using constraint_system_marshalling_type =
        nil::crypto3::marshalling::types::plonk_constraint_system<TTypeBase, ConstraintSystemType>;

    using ColumnType = nil::crypto3::zk::snark::plonk_column<BlueprintFieldType>;
    using AssignmentTableType =
        nil::crypto3::zk::snark::plonk_table<BlueprintFieldType, ColumnType>;

    const std::size_t Lambda = vm.count("lambda")? vm["lambda"].as<std::size_t>() : parameters_policy::lambda;
    const std::size_t max_quotient_chunks = vm.count("max-quotient-chunks")? vm["max-quotient-chunks"].as<std::size_t>() : 0;
    const std::size_t expand_factor = vm.count("expand-factor")? vm["expand-factor"].as<std::size_t>() : 2;

    using Hash = typename parameters_policy::hash;
    using circuit_params = nil::crypto3::zk::snark::placeholder_circuit_params<
        BlueprintFieldType
    >;

    using lpc_params_type = nil::crypto3::zk::commitments::list_polynomial_commitment_params<
        Hash,
        Hash,
        2
    >;
    using lpc_type = nil::crypto3::zk::commitments::list_polynomial_commitment<BlueprintFieldType, lpc_params_type>;
    using lpc_scheme_type = typename nil::crypto3::zk::commitments::lpc_commitment_scheme<lpc_type>;
    using placeholder_params = nil::crypto3::zk::snark::placeholder_params<circuit_params, lpc_scheme_type>;
    using policy_type = nil::crypto3::zk::snark::detail::placeholder_policy<BlueprintFieldType, placeholder_params>;

    using CommonDataType = typename nil::crypto3::zk::snark::placeholder_public_preprocessor<BlueprintFieldType, placeholder_params>::preprocessed_data_type::common_data_type;
    using common_data_marshalling_type = nil::crypto3::marshalling::types::placeholder_common_data<TTypeBase, CommonDataType>;
    using assignment_table_marshalling_type =
        nil::crypto3::marshalling::types::plonk_assignment_table<TTypeBase, AssignmentTableType>;

    using ColumnsRotationsType = std::vector<std::set<int>>;
    using ProfilingType = nil::blueprint::minimized_profiling_plonk_circuit<BlueprintFieldType>;

    std::optional<TableDescriptionType> desc;
    std::optional<ConstraintSystemType> constraint_system;
    std::optional<AssignmentTableType> assignment_table;
    std::optional<CommonDataType> common_data;
    {
        auto marshalled_value = decode_marshalling_from_file<constraint_system_marshalling_type>(circuit_file_name);
        if (!marshalled_value) {
            return false;
        }
        constraint_system.emplace(
            nil::crypto3::marshalling::types::make_plonk_constraint_system<Endianness, ConstraintSystemType>(
                *marshalled_value
            )
        );
    }
    if( !constraint_system ) return false;
    std::vector<std::size_t> public_input_sizes(PublicInputColumns);
    public_input_sizes = (*constraint_system).public_input_sizes();

    if( vm.count("assignment-table") ){

        AssignmentTableType table;
        zk::snark::plonk_table_description<BlueprintFieldType> description (
            WitnessColumns, PublicInputColumns, ConstantColumns, SelectorColumns);

        assignment_table_marshalling_type marshalled_table_data =
            extract_table_from_binary_file<assignment_table_marshalling_type>
                (assignment_table_file_name);

        std::tie(description, table) =
            nil::crypto3::marshalling::types::make_assignment_table<Endianness, AssignmentTableType>(
                marshalled_table_data
            );


        assignment_table.emplace(table);
        desc.emplace(description);
    }

    if( vm.count("common-data") ){
        auto marshalled_value = decode_marshalling_from_file<common_data_marshalling_type>(common_data_file_name);
        if (!marshalled_value) {
            return false;
        }
        auto c_data = nil::crypto3::marshalling::types::make_placeholder_common_data<Endianness, CommonDataType>(
            *marshalled_value
        );
        desc.emplace(c_data.desc);
        common_data.emplace(c_data);
    }

    if(!desc){
        std::cerr << "Table description is not provided" << std::endl;
        return 1;
    }
    std::cout << "WitnessColumns = " << desc->witness_columns << std::endl;
    std::cout << "PublicInputColumns = " << desc->public_input_columns << std::endl;
    std::cout << "ConstantColumns = " << desc->constant_columns << std::endl;
    std::cout << "SelectorColumns = " << desc->selector_columns << std::endl;

    std::vector<std::set<int>> columns_rotations;

    std::size_t table_rows_log = std::ceil(std::log2(desc->rows_amount));
    using lpc_params_type = nil::crypto3::zk::commitments::list_polynomial_commitment_params<
        Hash,
        Hash,
        2
    >;

    // TODO: grinding bits
    typename lpc_type::fri_type::params_type fri_params(
        1, table_rows_log, Lambda, expand_factor
    );
    lpc_scheme_type lpc_scheme(fri_params);

    if( mode == "gen-verifier" && !assignment_table && !common_data ){
        std::cerr << "Invalid command line argument - assignment table file name or common data file name should be specified" << std::endl;
        std::cout << options_desc << std::endl;
        return 1;
    }
    if( mode == "gen-input" ){
        if( !assignment_table ){
            std::cerr << "Invalid command line argument - assignment table file name should be specified for proof generation" << std::endl;
            std::cout << options_desc << std::endl;
            return 1;
        }
        if( common_data ){
            std::cerr << "Common data will be ignored for proof generation" << std::endl;
        }
    }

    if( mode == "gen-input" || (mode == "gen-verifier" && !common_data) ){
        std::cout << "Preprocessing public data..." << std::endl;
        typename nil::crypto3::zk::snark::placeholder_public_preprocessor<
            BlueprintFieldType, placeholder_params>::preprocessed_data_type public_preprocessed_data =
        nil::crypto3::zk::snark::placeholder_public_preprocessor<BlueprintFieldType, placeholder_params>::process(
            *constraint_system, assignment_table->public_table(), *desc, lpc_scheme, max_quotient_chunks);
        common_data.emplace(public_preprocessed_data.common_data);

        if (mode == "gen-input") {
            std::cout << "Preprocessing private data..." << std::endl;
            typename nil::crypto3::zk::snark::placeholder_private_preprocessor<
                BlueprintFieldType, placeholder_params>::preprocessed_data_type private_preprocessed_data =
                nil::crypto3::zk::snark::placeholder_private_preprocessor<BlueprintFieldType, placeholder_params>::process(
                    *constraint_system, assignment_table->private_table(), *desc
                );

            std::cout << "Generating proof..." << std::endl;
            using ProofType = nil::crypto3::zk::snark::placeholder_proof<BlueprintFieldType, placeholder_params>;
            ProofType proof = nil::crypto3::zk::snark::placeholder_prover<BlueprintFieldType, placeholder_params>::process(
                public_preprocessed_data, private_preprocessed_data, *desc, *constraint_system, lpc_scheme);
            std::cout << "Proof generated" << std::endl;

            if( !vm.count("skip-verification") ) {
                std::cout << "Verifying proof..." << std::endl;
                bool verification_result =
                    nil::crypto3::zk::snark::placeholder_verifier<BlueprintFieldType, placeholder_params>::process(
                        public_preprocessed_data.common_data, proof, *desc, *constraint_system, lpc_scheme
                    );

                ASSERT_MSG(verification_result, "Proof is not verified" );
                std::cout << "Proof is verified" << std::endl;
            }

            std::string inp_path = output_folder_path + "/placeholder_verifier.inp";
            std::ofstream output_file;
            output_file.open(inp_path);
            output_file << nil::blueprint::recursive_verifier_generator<
                placeholder_params,
                nil::crypto3::zk::snark::placeholder_proof<BlueprintFieldType, placeholder_params>,
                typename nil::crypto3::zk::snark::placeholder_public_preprocessor<BlueprintFieldType, placeholder_params>::preprocessed_data_type::common_data_type
            >(*desc).generate_input(
                assignment_table->public_inputs(), proof, public_input_sizes
            );
            output_file.close();
        }
    }
    if( mode == "gen-verifier"){
        std::string cpp_path = output_folder_path + "/placeholder_verifier.cpp";
        std::ofstream output_file;
        output_file.open(cpp_path);
        output_file << nil::blueprint::recursive_verifier_generator<
            placeholder_params,
            nil::crypto3::zk::snark::placeholder_proof<BlueprintFieldType, placeholder_params>,
            typename nil::crypto3::zk::snark::placeholder_public_preprocessor<BlueprintFieldType, placeholder_params>::preprocessed_data_type::common_data_type
        >(*desc).generate_recursive_verifier(
            *constraint_system, *common_data, public_input_sizes
        );
        output_file.close();
        return 0;
    }

    return 0;
}
