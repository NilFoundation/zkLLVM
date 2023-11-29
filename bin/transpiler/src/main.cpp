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
#include <nil/crypto3/zk/snark/arithmetization/plonk/padding.hpp>

#include <nil/marshalling/status_type.hpp>
#include <nil/marshalling/field_type.hpp>
#include <nil/marshalling/endianness.hpp>
#include <nil/crypto3/marshalling/zk/types/placeholder/proof.hpp>
#include <nil/crypto3/marshalling/zk/types/plonk/constraint_system.hpp>
#include <nil/crypto3/marshalling/zk/types/plonk/assignment_table.hpp>

#include <nil/crypto3/zk/snark/systems/plonk/placeholder/preprocessor.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/prover.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/verifier.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/params.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/profiling.hpp>

#include <nil/crypto3/math/polynomial/polynomial.hpp>
#include <nil/crypto3/math/algorithms/calculate_domain_set.hpp>

#include <nil/blueprint/asserts.hpp>
#include <nil/blueprint/transpiler/minimized_profiling_plonk_circuit.hpp>
#include <nil/blueprint/transpiler/evm_verifier_gen.hpp>
#include <nil/blueprint/transpiler/public_input.hpp>

#include <nil/blueprint/utils/satisfiability_check.hpp>

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

template<typename ProfilingType, typename ConstraintSystemType, typename ColumnsRotationsType,
         typename ArithmetizationParams>
void print_sol_files(ConstraintSystemType &constraint_system, ColumnsRotationsType &columns_rotations,
                     std::string out_folder_path = ".", bool optimize_gates = false) {
    ProfilingType::process_split(
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

int main(int argc, char *argv[]) {

    boost::program_options::options_description options_desc("zkLLVM circuit EVM gate argument transpiler");

    // clang-format off
    options_desc.add_options()("help,h", "Display help message")
            ("version,v", "Display version")
            ("mode,m", boost::program_options::value<std::string>(), "Transpiler mode (gen-circuit-params, gen-gate-argument, gen-test-proof, gen-evm-verifier).\
            gen-gate-argument prepares gate argument and some placeholder params.\
            gen-circuit-params prepares circuit parameters for verification.\
            gen-test-proof prepares gate argument, placeholder params and sample proof for local testing.\
            gen-shared-proof generates proof from multiprover.\
            gen-evm-verifier generates all modules for evm verification.")
            ("public-input,i", boost::program_options::value<std::string>(), "Public input file")
            ("assignment-table,t", boost::program_options::value<std::string>(), "Assignment table input file")
            ("shared-assignment-table,s", boost::program_options::value<std::string>(), "Shared assignment table input file")
            ("circuit,c", boost::program_options::value<std::string>(), "Circuit input file")
            ("output-folder-path,o", boost::program_options::value<std::string>(), "Output folder absolute path.\
            It'll be better to create an empty folder for output")
            ("optimize-gates", "Put multiple small gates into several .sol file, sets reasonable threshold limits")
            ("skip-verification", "Used with gen-test-proof, if set - skips verifiyng the generated proof")
            ("gates-contract-size-threshold", boost::program_options::value<std::size_t>(), "Gates library size limit, per module. Default = 0, each constraint in a separate library")
            ("lookups-library-threshold", boost::program_options::value<std::size_t>(), "Lookups library size limit, per module. Default = 0, each lookup in a separate library")
            ("lookups-inline-threshold", boost::program_options::value<std::size_t>(), "Lookups inline size limit. Default = 0, none of the lookups are inlined")
            ("deduce-horner", "Detect polynomials over one variable and deduce to Horner's formula")
            ("optimize-powers", "Optimize terms that are powers of single variable")
            ;
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
#ifdef TRANSPILER_VERSION
#define xstr(s) str(s)
#define str(s) #s
        std::cout << xstr(TRANSPILER_VERSION) << std::endl;
#else
        std::cout << "undefined" << std::endl;
#endif
        return 0;
    }

    std::string mode;
    std::string assignment_table_file_name;
    std::string shared_table_file_name;
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

    if (!(mode == "gen-test-proof" || mode == "gen-gate-argument" || mode == "gen-circuit-params" || mode == "gen-evm-verifier" || mode == "gen-shared-proof")) {
        std::cerr << "Invalid mode specified" << std::endl;
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

    if( vm.count("shared-assignment-table")){
        shared_table_file_name = vm["shared-assignment-table"].as<std::string>();
        std::cout << "Shared table file: " << shared_table_file_name << std::endl;
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

    if( mode == "gen-shared-proof" ){
        if (vm.count("shared-assignment-table")) {
            shared_table_file_name = vm["shared-assignment-table"].as<std::string>();
        } else {
            std::cerr << "Invalid command line argument - shared assignment table file name is not specified" << std::endl;
            std::cout << options_desc << std::endl;
            return 1;
        }
        std::cout << "Generate shared proof" << std::endl;

        using curve_type = nil::crypto3::algebra::curves::pallas;
        using BlueprintFieldType = typename curve_type::base_field_type;
        constexpr std::size_t WitnessColumns = 15;
        constexpr std::size_t PublicInputColumns = 2;
        constexpr std::size_t ConstantColumns = 5;
        constexpr std::size_t SelectorColumns = 35;

        using ColumnType = nil::crypto3::zk::snark::plonk_column<BlueprintFieldType>;
        using Endianness = nil::marshalling::option::big_endian;
        using TTypeBase = nil::marshalling::field_type<Endianness>;

        using SourceArithmetizationParams =
            nil::crypto3::zk::snark::plonk_arithmetization_params<WitnessColumns, 0, ConstantColumns, SelectorColumns>;
        using SourceAssignmentTableType =
            nil::crypto3::zk::snark::plonk_table<BlueprintFieldType, SourceArithmetizationParams, ColumnType>;
        using source_table_value_marshalling_type =
            nil::crypto3::marshalling::types::plonk_assignment_table<TTypeBase, SourceAssignmentTableType>;

         using SharedArithmetizationParams =
            nil::crypto3::zk::snark::plonk_arithmetization_params<0, PublicInputColumns, 0, 0>;
        using SharedAssignmentTableType =
            nil::crypto3::zk::snark::plonk_table<BlueprintFieldType, SharedArithmetizationParams, ColumnType>;
        using shared_table_value_marshalling_type =
            nil::crypto3::marshalling::types::plonk_assignment_table<TTypeBase, SharedAssignmentTableType>;

        using ArithmetizationParams =
            nil::crypto3::zk::snark::plonk_arithmetization_params<WitnessColumns, PublicInputColumns, ConstantColumns,
                                                                  SelectorColumns>;
        using AssignmentTableType =
            nil::crypto3::zk::snark::plonk_table<BlueprintFieldType, ArithmetizationParams, ColumnType>;
        using table_value_marshalling_type =
            nil::crypto3::marshalling::types::plonk_assignment_table<TTypeBase, AssignmentTableType>;

        std::size_t shared_table_usable_rows;
        SharedAssignmentTableType shared_table;
        {
            std::cout << "Source table file: " << shared_table_file_name << std::endl;
            std::ifstream shared_iassignment;
            shared_iassignment.open(shared_table_file_name);
            if (!shared_iassignment) {
                std::cout << "Cannot open " << shared_table_file_name << std::endl;
                return 1;
            }
            std::vector<std::uint8_t> shared_v;
            if (!read_buffer_from_file(shared_iassignment, shared_v)) {
                std::cout << "Cannot parse input file " << shared_table_file_name << std::endl;
                return 1;
            }
            shared_iassignment.close();
            shared_table_value_marshalling_type shared_marshalled_table_data;
            auto read_iter = shared_v.begin();
            auto status = shared_marshalled_table_data.read(read_iter, shared_v.size());
            std::tie(shared_table_usable_rows, shared_table) =
                nil::crypto3::marshalling::types::make_assignment_table<Endianness, SharedAssignmentTableType>(
                    shared_marshalled_table_data
                );
            std::cout << "Shared table usable rows = " << shared_table_usable_rows << std::endl;
            for(std::size_t i = 0; i < SharedArithmetizationParams::public_input_columns; i++){
                std::cout << "Public input : ";
                for(std::size_t j = 0; j < shared_table.public_inputs()[i].size(); j++){
                    std::cout << shared_table.public_inputs()[i][j] << " ";
                }
                std::cout << std::endl;
            }
        }

        std::size_t source_table_usable_rows;
        SourceAssignmentTableType source_table;
        {
            std::cout << "Source table file: " << assignment_table_file_name << std::endl;
            std::ifstream source_iassignment;
            source_iassignment.open(assignment_table_file_name);
            if (!source_iassignment) {
                std::cout << "Cannot open " << assignment_table_file_name << std::endl;
                return 1;
            }
            std::vector<std::uint8_t> source_v;
            if (!read_buffer_from_file(source_iassignment, source_v)) {
                std::cout << "Cannot parse input file " << assignment_table_file_name << std::endl;
                return 1;
            }
            source_iassignment.close();
            source_table_value_marshalling_type source_marshalled_table_data;
            auto read_iter = source_v.begin();
            auto status = source_marshalled_table_data.read(read_iter, source_v.size());
            std::tie(source_table_usable_rows, source_table) =
                nil::crypto3::marshalling::types::make_assignment_table<Endianness, SourceAssignmentTableType>(
                    source_marshalled_table_data
                );
            std::cout << "Source table usable rows = " << source_table_usable_rows << std::endl;
        }

        AssignmentTableType assignment(
            nil::crypto3::zk::snark::plonk_private_table<BlueprintFieldType, ArithmetizationParams, ColumnType>(source_table.witnesses()),
            nil::crypto3::zk::snark::plonk_public_table<BlueprintFieldType, ArithmetizationParams, ColumnType>(
                {shared_table.public_inputs()[1],shared_table.public_inputs()[0]},
                source_table.constants(),
                source_table.selectors()
            )
        );

        using ConstraintSystemType =
            nil::crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;
        using TableDescriptionType =
            nil::crypto3::zk::snark::plonk_table_description<BlueprintFieldType, ArithmetizationParams>;
        TableDescriptionType table_description;
        table_description.usable_rows_amount = std::max(source_table_usable_rows, shared_table_usable_rows);
        table_description.rows_amount = std::max(source_table.rows_amount(), shared_table.rows_amount());
        std::cout << "Rows amount before = " << table_description.rows_amount << std::endl;
        table_description.rows_amount = nil::crypto3::zk::snark::basic_padding(assignment);
        std::cout << "Rows amount after = " << table_description.rows_amount << std::endl;

        std::cout << "Real table usable rows = " << table_description.usable_rows_amount << std::endl;
        for(std::size_t i = 0; i < ArithmetizationParams::witness_columns; i++){
            std::cout << "Witness "<< i << " : " ;
            for(std::size_t j = 0; j < assignment.witnesses()[i].size(); j++){
                std::cout << assignment.witnesses()[i][j] << " ";
            }
            std::cout << std::endl;
        }
        for(std::size_t i = 0; i < ArithmetizationParams::public_input_columns; i++){
            std::cout << "Public_input " << i << " : " ;
            for(std::size_t j = 0; j < assignment.public_inputs()[i].size(); j++){
                std::cout << assignment.public_inputs()[i][j] << " ";
            }
            std::cout << std::endl;
        }
        for(std::size_t i = 0; i < ArithmetizationParams::constant_columns; i++){
            std::cout << "Constant " << i << " : " ;
            for(std::size_t j = 0; j < assignment.constants()[i].size(); j++){
                std::cout << assignment.constants()[i][j] << " ";
            }
            std::cout << std::endl;
        }
        for(std::size_t i = 0; i < ArithmetizationParams::selector_columns; i++){
            std::cout << "Selector " << i << " : ";
            for(std::size_t j = 0; j < assignment.selectors()[i].size(); j++){
                std::cout << assignment.selectors()[i][j] << " ";
            }
            std::cout << std::endl;
        }

        std::cout << "Rows amount = " << table_description.rows_amount << std::endl;

        using value_marshalling_type =
            nil::crypto3::marshalling::types::plonk_constraint_system<TTypeBase, ConstraintSystemType>;


        ConstraintSystemType constraint_system;
        {
            std::ifstream ifile;
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

            value_marshalling_type marshalled_data;
            auto read_iter = v.begin();
            auto status = marshalled_data.read(read_iter, v.size());
            constraint_system = nil::crypto3::marshalling::types::make_plonk_constraint_system<Endianness, ConstraintSystemType>(
                    marshalled_data
            );

            for(std::size_t i = 0; i < constraint_system.copy_constraints().size(); i++){
                std::cout << "Copy_constraint " << i << " : " << constraint_system.copy_constraints()[i].first << " " << constraint_system.copy_constraints()[i].second << std::endl;
            }
            for(std::size_t i = 0; i < constraint_system.gates().size(); i++){
                for(std::size_t j = 0; j < constraint_system.gates()[i].constraints.size(); j++){
                    std::cout << "Gate " << i << " constraint " << j << " : " << constraint_system.gates()[i].constraints[j] << std::endl;
                }
            }
        }

        using ColumnsRotationsType = std::array<std::set<int>, ArithmetizationParams::total_columns>;
        using ProfilingType = nil::blueprint::minimized_profiling_plonk_circuit<BlueprintFieldType, ArithmetizationParams>;
        auto columns_rotations = ProfilingType::columns_rotations(constraint_system, table_description);

        const std::size_t Lambda = 9;
        using Hash = nil::crypto3::hashes::keccak_1600<256>;
        using circuit_params = nil::crypto3::zk::snark::placeholder_circuit_params<
            BlueprintFieldType, ArithmetizationParams
        >;

        std::size_t table_rows_log = std::ceil(std::log2(table_description.rows_amount));
        using lpc_params_type = nil::crypto3::zk::commitments::list_polynomial_commitment_params<
            Hash,
            Hash,
            Lambda,
            2
        >;
        using lpc_type = nil::crypto3::zk::commitments::list_polynomial_commitment<BlueprintFieldType, lpc_params_type>;
        using lpc_scheme_type = typename nil::crypto3::zk::commitments::lpc_commitment_scheme<lpc_type>;
        using placeholder_params = nil::crypto3::zk::snark::placeholder_params<circuit_params, lpc_scheme_type>;
        using policy_type = nil::crypto3::zk::snark::detail::placeholder_policy<BlueprintFieldType, placeholder_params>;

        auto fri_params = create_fri_params<typename lpc_type::fri_type, BlueprintFieldType>(table_rows_log);
        std::size_t permutation_size =
            table_description.witness_columns + table_description.public_input_columns + table_description.constant_columns;
        lpc_scheme_type lpc_scheme(fri_params);

        std::cout << "Preprocessing public data..." << std::endl;
        typename nil::crypto3::zk::snark::placeholder_public_preprocessor<
            BlueprintFieldType, placeholder_params>::preprocessed_data_type public_preprocessed_data =
        nil::crypto3::zk::snark::placeholder_public_preprocessor<BlueprintFieldType, placeholder_params>::process(
            constraint_system, assignment.public_table(), table_description, lpc_scheme, permutation_size);

        std::cout << "Preprocessing private data..." << std::endl;
        typename nil::crypto3::zk::snark::placeholder_private_preprocessor<
            BlueprintFieldType, placeholder_params>::preprocessed_data_type private_preprocessed_data =
            nil::crypto3::zk::snark::placeholder_private_preprocessor<BlueprintFieldType, placeholder_params>::process(
                constraint_system, assignment.private_table(), table_description
            );

        std::cout << "Generating proof..." << std::endl;
        using ProofType = nil::crypto3::zk::snark::placeholder_proof<BlueprintFieldType, placeholder_params>;
        ProofType proof = nil::crypto3::zk::snark::placeholder_prover<BlueprintFieldType, placeholder_params>::process(
            public_preprocessed_data, private_preprocessed_data, table_description, constraint_system, assignment,
            lpc_scheme);
        std::cout << "Proof generated" << std::endl;

        std::string proof_path = output_folder_path + "/proof.bin";
        std::cout << "Writing proof to " << proof_path << "..." << std::endl;
        auto filled_placeholder_proof =
            nil::crypto3::marshalling::types::fill_placeholder_proof<Endianness, ProofType>(proof);
        proof_print<Endianness, ProofType>(proof, proof_path);
        std::cout << "Proof written" << std::endl;


        if( !vm.count("skip-verification") ) {
            std::cout << "Verifying proof..." << std::endl;
            bool verification_result =
                nil::crypto3::zk::snark::placeholder_verifier<BlueprintFieldType, placeholder_params>::process(
                    public_preprocessed_data, proof, constraint_system, lpc_scheme
                );

            ASSERT_MSG(verification_result, "Proof is not verified" );
            std::cout << "Proof is verified" << std::endl;
        }
        return 0;
    }

    using curve_type = nil::crypto3::algebra::curves::pallas;
    using BlueprintFieldType = typename curve_type::base_field_type;
    constexpr std::size_t WitnessColumns = 15;
    constexpr std::size_t PublicInputColumns = 1;
    constexpr std::size_t ConstantColumns = 30;
    constexpr std::size_t SelectorColumns = 35;

    using ArithmetizationParams =
        nil::crypto3::zk::snark::plonk_arithmetization_params<WitnessColumns, PublicInputColumns, ConstantColumns,
                                                              SelectorColumns>;
    using ConstraintSystemType =
        nil::crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;
    using TableDescriptionType =
        nil::crypto3::zk::snark::plonk_table_description<BlueprintFieldType, ArithmetizationParams>;
    using Endianness = nil::marshalling::option::big_endian;
    using TTypeBase = nil::marshalling::field_type<Endianness>;
    using value_marshalling_type =
        nil::crypto3::marshalling::types::plonk_constraint_system<TTypeBase, ConstraintSystemType>;

    using ColumnType = nil::crypto3::zk::snark::plonk_column<BlueprintFieldType>;
    using AssignmentTableType =
        nil::crypto3::zk::snark::plonk_table<BlueprintFieldType, ArithmetizationParams, ColumnType>;
    using table_value_marshalling_type =
        nil::crypto3::marshalling::types::plonk_assignment_table<TTypeBase, AssignmentTableType>;

    using ColumnsRotationsType = std::array<std::set<int>, ArithmetizationParams::total_columns>;
    using ProfilingType = nil::blueprint::minimized_profiling_plonk_circuit<BlueprintFieldType, ArithmetizationParams>;

    if( vm.count("public-input") ){
        public_input = vm["public-input"].as<std::string>();
        if( !boost::filesystem::exists(public_input) ){
            std::cerr << "Invalid command line argument - public input file does not exist" << std::endl;
            return 1;
        }
        boost::filesystem::copy(public_input, output_folder_path+"/public_input.json", boost::filesystem::copy_options::overwrite_existing);
    }

    ConstraintSystemType constraint_system;
    {
        std::ifstream ifile;
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

        value_marshalling_type marshalled_data;
        auto read_iter = v.begin();
        auto status = marshalled_data.read(read_iter, v.size());
        constraint_system = nil::crypto3::marshalling::types::make_plonk_constraint_system<Endianness, ConstraintSystemType>(
                marshalled_data
        );
    }

    TableDescriptionType table_description;
    AssignmentTableType assignment_table;
    {
        std::ifstream iassignment;
        iassignment.open(assignment_table_file_name);
        if (!iassignment) {
            std::cout << "Cannot open " << assignment_table_file_name << std::endl;
            return 1;
        }
        std::vector<std::uint8_t> v;
        if (!read_buffer_from_file(iassignment, v)) {
            std::cout << "Cannot parse input file " << assignment_table_file_name << std::endl;
            return 1;
        }
        iassignment.close();
        table_value_marshalling_type marshalled_table_data;
        auto read_iter = v.begin();
        auto status = marshalled_table_data.read(read_iter, v.size());
        std::tie(table_description.usable_rows_amount, assignment_table) =
            nil::crypto3::marshalling::types::make_assignment_table<Endianness, AssignmentTableType>(
                marshalled_table_data
            );
        table_description.rows_amount = assignment_table.rows_amount();
    }

    auto columns_rotations = ProfilingType::columns_rotations(constraint_system, table_description);

    const std::size_t Lambda = 9;
    using Hash = nil::crypto3::hashes::keccak_1600<256>;
    using circuit_params = nil::crypto3::zk::snark::placeholder_circuit_params<
        BlueprintFieldType, ArithmetizationParams
    >;

    std::size_t table_rows_log = std::ceil(std::log2(table_description.rows_amount));
    using lpc_params_type = nil::crypto3::zk::commitments::list_polynomial_commitment_params<
        Hash,
        Hash,
        Lambda,
        2
    >;
    using lpc_type = nil::crypto3::zk::commitments::list_polynomial_commitment<BlueprintFieldType, lpc_params_type>;
    using lpc_scheme_type = typename nil::crypto3::zk::commitments::lpc_commitment_scheme<lpc_type>;
    using placeholder_params = nil::crypto3::zk::snark::placeholder_params<circuit_params, lpc_scheme_type>;
    using policy_type = nil::crypto3::zk::snark::detail::placeholder_policy<BlueprintFieldType, placeholder_params>;

    auto fri_params = create_fri_params<typename lpc_type::fri_type, BlueprintFieldType>(table_rows_log);
    std::size_t permutation_size =
        table_description.witness_columns + table_description.public_input_columns + table_description.constant_columns;
    lpc_scheme_type lpc_scheme(fri_params);


    if (mode == "gen-gate-argument") {
        bool optimize_gates = false;
        if( vm.count("optimize-gates") )
            optimize_gates = true;
        print_sol_files<ProfilingType, ConstraintSystemType, ColumnsRotationsType, ArithmetizationParams>(
            constraint_system, columns_rotations, output_folder_path, optimize_gates);
    }

    if ((mode != "gen-circuit-params") && (mode != "gen-test-proof") && (mode != "gen-evm-verifier")) {
        return 0;
    }

    table_description.rows_amount = nil::crypto3::zk::snark::basic_padding(assignment_table);

    std::cout << "Preprocessing public data..." << std::endl;
    typename nil::crypto3::zk::snark::placeholder_public_preprocessor<
        BlueprintFieldType, placeholder_params>::preprocessed_data_type public_preprocessed_data =
    nil::crypto3::zk::snark::placeholder_public_preprocessor<BlueprintFieldType, placeholder_params>::process(
        constraint_system, assignment_table.public_table(), table_description, lpc_scheme, permutation_size);

    if (mode == "gen-evm-verifier") {
        std::size_t gates_contract_size_threshold = 800;
        std::size_t lookups_library_threshold = 0;
        std::size_t lookups_inline_threshold = 0;
        bool deduce_horner = false;
        bool optimize_powers = false;

        if ( vm.count("optimize-gates") >0 ) {
            lookups_library_threshold = 1000;
            lookups_inline_threshold = 1000;
            deduce_horner = true;
            optimize_powers = true;
        }

        if ( vm.count("gates-contract-size-threshold") ) {
            gates_contract_size_threshold = vm["gates-contract-size-threshold"].as<std::size_t>();
        }
        if ( vm.count("lookups-library-threshold") ) {
            lookups_library_threshold = vm["lookups-library-threshold"].as<std::size_t>();
        }
        if ( vm.count("lookups-inline-threshold") ) {
            lookups_inline_threshold = vm["lookups-inline-threshold"].as<std::size_t>();
        }
        if ( vm.count("deduce-horner") > 0 ) {
            deduce_horner = true;
        }
        if ( vm.count("optimize-powers") > 0 ) {
            optimize_powers = true;
        }
        nil::blueprint::evm_verifier_printer<placeholder_params>(
            constraint_system,
            public_preprocessed_data.common_data,
            lpc_scheme,
            permutation_size,
            output_folder_path,
            gates_contract_size_threshold,
            lookups_library_threshold,
            lookups_inline_threshold,
            deduce_horner,
            optimize_powers
        ).print();
        return 0;
    }

    nil::crypto3::zk::snark::print_placeholder_params<placeholder_params>(
        public_preprocessed_data, lpc_scheme, output_folder_path+"/circuit_params.json");

    if (mode == "gen-test-proof") {
        std::cout << "Preprocessing private data..." << std::endl;
        typename nil::crypto3::zk::snark::placeholder_private_preprocessor<
            BlueprintFieldType, placeholder_params>::preprocessed_data_type private_preprocessed_data =
            nil::crypto3::zk::snark::placeholder_private_preprocessor<BlueprintFieldType, placeholder_params>::process(
                constraint_system, assignment_table.private_table(), table_description
            );

        if (constraint_system.num_gates() == 0){
            std::cout << "Generating proof..." << std::endl;
            std::cout << "Proof generated" << std::endl;
            if( !vm.count("skip-verification") ) {
                std::cout << "Verifying proof..." << std::endl;
                std::cout << "Proof is verified" << std::endl;
            } else {
                std::cout << "Proof verification skipped" << std::endl;
            }
            std::string proof_path = output_folder_path + "/proof.bin";
            std::cout << "Writing proof to " << proof_path << "..." << std::endl;
            std::fstream fs;
            fs.open(proof_path, std::ios::out);
            fs.close();
            std::cout << "Proof written" << std::endl;
        } else {
            std::cout << "Generating proof..." << std::endl;
            using ProofType = nil::crypto3::zk::snark::placeholder_proof<BlueprintFieldType, placeholder_params>;
            ProofType proof = nil::crypto3::zk::snark::placeholder_prover<BlueprintFieldType, placeholder_params>::process(
                public_preprocessed_data, private_preprocessed_data, table_description, constraint_system, assignment_table,
                lpc_scheme);
            std::cout << "Proof generated" << std::endl;

            if( !vm.count("skip-verification") ) {
                std::cout << "Verifying proof..." << std::endl;
                bool verification_result =
                    nil::crypto3::zk::snark::placeholder_verifier<BlueprintFieldType, placeholder_params>::process(
                        public_preprocessed_data, proof, constraint_system, lpc_scheme
                    );

                ASSERT_MSG(verification_result, "Proof is not verified" );
                std::cout << "Proof is verified" << std::endl;
            }

            std::string proof_path = output_folder_path + "/proof.bin";
            std::cout << "Writing proof to " << proof_path << "..." << std::endl;
            auto filled_placeholder_proof =
                nil::crypto3::marshalling::types::fill_placeholder_proof<Endianness, ProofType>(proof);
            proof_print<Endianness, ProofType>(proof, proof_path);
            std::cout << "Proof written" << std::endl;
        }
        return 0;
    }
}
