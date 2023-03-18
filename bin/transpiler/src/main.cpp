#include <iostream>
#include <fstream>
#include <vector>
#include <random>

#include <nil/crypto3/algebra/curves/pallas.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/pallas.hpp>

#include <nil/crypto3/zk/snark/arithmetization/plonk/params.hpp>
#include <nil/crypto3/zk/snark/arithmetization/plonk/constraint_system.hpp>

#include <nil/marshalling/status_type.hpp>
#include <nil/marshalling/field_type.hpp>
#include <nil/marshalling/endianness.hpp>
#include <nil/crypto3/marshalling/zk/types/placeholder/proof.hpp>
#include <nil/crypto3/marshalling/zk/types/plonk/constraint_system.hpp>

#include <nil/crypto3/zk/snark/systems/plonk/placeholder/preprocessor.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/prover.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/verifier.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/params.hpp>

#include <nil/crypto3/math/polynomial/polynomial.hpp>
#include <nil/crypto3/math/algorithms/calculate_domain_set.hpp>

#include <nil/blueprint/transpiler/minimized_profiling_plonk_circuit.hpp>

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
void print_sol_file(std::ostream &os, ConstraintSystemType &constraint_system,
                    ColumnsRotationsType &columns_rotations) {
    os << "pragma solidity >=0.8.4;\n\n"
       << "import \"../types.sol\";\n"
       << "import \"../basic_marshalling.sol\";\n"
       << "import \"../commitments/batched_lpc_verifier.sol\";\n"
       << "import \"../interfaces/gate_argument.sol\";\n"
       << "\n"
       << "contract gate_argument_gen is IGateArgument {\n"
       << "    uint256 constant WITNESSES_N = " << ArithmetizationParams::WitnessColumns << ";\n"
       << "    uint256 constant GATES_N = " << constraint_system.gates().size() << ";\n"
       << "\n"
       << "    uint256 constant MODULUS_OFFSET = 0x0;\n"
       << "    uint256 constant THETA_OFFSET = 0x20;\n"
       << "    uint256 constant CONSTRAINT_EVAL_OFFSET = 0x40;\n"
       << "    uint256 constant GATE_EVAL_OFFSET = 0x60;\n"
       << "    uint256 constant WITNESS_EVALUATIONS_OFFSET = 0x80;\n"
       << "    uint256 constant SELECTOR_EVALUATIONS_OFFSET = 0xa0;\n"
       << "    uint256 constant EVAL_PROOF_WITNESS_OFFSET_OFFSET = 0xc0;\n"
       << "    uint256 constant EVAL_PROOF_SELECTOR_OFFSET_OFFSET = 0xe0;\n"
       << "    uint256 constant GATES_EVALUATION_OFFSET = 0x100;\n"
       << "    uint256 constant THETA_ACC_OFFSET = 0x120;\n"
       << "    uint256 constant SELECTOR_EVALUATIONS_OFFSET_OFFSET = 0x140;\n"
       << "    uint256 constant OFFSET_OFFSET = 0x160;\n"
       << "\n"
       << "    function evaluate_gates_be(\n"
       << "        bytes calldata blob,\n"
       << "        types.gate_argument_local_vars memory gate_params,\n"
       << "        types.arithmetization_params memory ar_params,\n"
       << "        int256[][] memory columns_rotations\n"
       << "    ) external pure returns (uint256 gates_evaluation) {\n"
       << "\n"
       << "        gate_params.offset = basic_marshalling.skip_length(\n"
       << "            batched_lpc_verifier.skip_to_z(\n"
       << "                blob,\n"
       << "                gate_params.eval_proof_witness_offset\n"
       << "            )\n"
       << "        );\n"
       << "        gate_params.witness_evaluations_offsets = new uint256[](WITNESSES_N);\n"
       << "        for (uint256 i = 0; i < WITNESSES_N; i++) {\n"
       << "            gate_params.witness_evaluations_offsets[i] = basic_marshalling\n"
       << "                .get_i_uint256_ptr_from_vector(blob, gate_params.offset, 0);\n"
       << "            gate_params.offset = basic_marshalling.skip_vector_of_uint256_be(\n"
       << "                blob,\n"
       << "                gate_params.offset\n"
       << "            );\n"
       << "        }\n"
       << "        gate_params.selector_evaluations = new uint256[](GATES_N);\n"
       << "        for (uint256 i = 0; i < GATES_N; i++) {\n"
       << "            gate_params.selector_evaluations[i] = batched_lpc_verifier\n"
       << "                .get_z_i_j_from_proof_be(\n"
       << "                    blob,\n"
       << "                    gate_params.eval_proof_selector_offset,\n"
       << "                    i + ar_params.permutation_columns + ar_params.permutation_columns + "
          "ar_params.constant_columns,\n"
       << "                    0\n"
       << "                );\n"
       << "        }\n"
       << "\n"
       << "        uint256 t = 0;\n"
       << "        assembly {\n"
       << "            let modulus := mload(gate_params)\n"
       << "            let theta_acc := 1\n"
       << "            function get_eval_i_by_rotation_idx(idx, rot_idx, ptr) -> result {"
       << "                result := calldataload(\n"
       << "                    add(\n"
       << "                        mload(add(add(ptr, 0x20), mul(0x20, idx))),\n"
       << "                        mul(0x20, rot_idx)\n"
       << "                    )\n"
       << "                )\n"
       << "            }\n"
       << "\n"
       << "            function get_selector_i(idx, ptr) -> result {\n"
       << "                result := mload(add(add(ptr, 0x20), mul(0x20, idx)))\n"
       << "            }\n"
       << "            let x1 := add(gate_params, CONSTRAINT_EVAL_OFFSET)\n"
       << "            let x2 := add(gate_params, WITNESS_EVALUATIONS_OFFSET)\n"
       << "            let x3 := get_eval_i_by_rotation_idx(0,0,mload(x2))\n"
       << "            let x4 := get_eval_i_by_rotation_idx(2,0,mload(x2))\n";

    ProfilingType::process(os, constraint_system, columns_rotations);
    os << "}\n}\n}" << std::endl;
}

template<typename ProfilingType, typename ConstraintSystemType, typename ColumnsRotationsType,
         typename ArithmetizationParams>
void print_sol_files(ConstraintSystemType &constraint_system, ColumnsRotationsType &columns_rotations,
                     std::string out_folder_path = ".") {
    ProfilingType::process_split(constraint_system, columns_rotations, out_folder_path);
}

template<typename FRIParamsType, typename TableDescriptionType, typename ColumnsRotationsType,
         typename ArithmetizationParams>
void print_params(FRIParamsType &fri_params, TableDescriptionType table_description,
                  ColumnsRotationsType &columns_rotations, std::string output_folder) {
    std::ofstream out;

    out.open(output_folder + "/circuit_params.json");
    out << "{\"arithmetization_params\":[" << ArithmetizationParams::witness_columns << ","
        << ArithmetizationParams::public_input_columns << "," << ArithmetizationParams::constant_columns << ","
        << ArithmetizationParams::selector_columns << "]," << std::endl
        << "\"columns_rotations\":[" << std::endl;
    for (size_t i = 0; i < columns_rotations.size(); i++) {
        if (i != 0)
            out << "," << std::endl;
        out << "[";
        for (size_t j = 0; j < columns_rotations[i].size(); j++) {
            if (j != 0)
                out << ",";
            out << columns_rotations[i][j];
        }
        out << "]";
    }
    out << "]," << std::endl;
    out << "\"r\":" << fri_params.r << "," << std::endl;
    out << "\"step_list\":[";
    for (size_t i = 0; i < fri_params.step_list.size(); i++) {
        if (i != 0)
            out << ",";
        out << fri_params.step_list[i];
    }
    out << "]," << std::endl;
    out << "\"D_omegas\":[" << std::endl;
    for (size_t i = 0; i < fri_params.D.size(); i++) {
        if (i != 0)
            out << "," << std::endl;
        out << fri_params.D[i]->get_domain_element(1).data;
    }
    out << "]," << std::endl;
    out << "\"rows_amount\":" << table_description.rows_amount << "," << std::endl;
    out << "\"max_degree\":" << fri_params.max_degree << "," << std::endl;
    out << "\"omega\":" << fri_params.D[0]->get_domain_element(1).data << std::endl;
    out << "}" << std::endl;
    out.close();
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
    if (argc < 2) {
        std::cout << "There are two commands:" << std::endl
                  << "gen_test_proof prepares gate argument, placeholder params and sample proof for testing"
                  << std::endl
                  << "gen_gate_argument prepares gate argument and some placeholder params" << std::endl;
        return 1;
    }

    std::string command = argv[1];
    std::string ifile_path;
    std::string iassignment_path;
    std::string ifolder_path;
    std::string ofolder_path;

    if (command == "gen_test_proof") {
        if (argc < 4) {
            std::cout << "First parameter marshalled plonk_constraint_system file path" << std::endl
                      << "1st parameter is a command gen_gate_argument or gen_test_proof" << std::endl
                      << "2nd parameter is input folder path" << std::endl
                      << "3th parameter is output folder path" << std::endl
                      << "It'll be better to create an empty folder for output" << std::endl
                      << "Copy output folder to evm-placeholder-verification" << std::endl;
            return 1;
        }
        if (argc > 4) {
            std::cout << "This program is not so complex yet" << std::endl;
            return 1;
        }
        std::cout << "proof" << std::endl;
        ifolder_path = argv[2];
        ofolder_path = argv[3];
    } else if (command == "gen_gate_argument") {
        if (argc < 4) {
            std::cout << "First parameter marshalled plonk_constraint_system file path" << std::endl
                      << "1st parameter is a command gen_gate_argument or gen_test_proof" << std::endl
                      << "2nd parameter is input folder path" << std::endl
                      << "3th parameter is output folder path" << std::endl
                      << "It'll be better to create an empty folder for output" << std::endl
                      << "Copy output folder to evm-placeholder-verification" << std::endl;
            return 1;
        }
        if (argc > 4) {
            std::cout << "This program is not so complex yet" << std::endl;
            return 0;
        }
        ifolder_path = argv[2];
        ofolder_path = argv[3];
        std::cout << "gate_argument" << std::endl;
    } else {
        std::cout << "Unknown command " << argv[1] << std::endl;
        return 1;
    }
    ifile_path = ifolder_path + "/circuit.bin";
    iassignment_path = ifolder_path + "/assignment_table.data";

    std::ifstream ifile;
    std::ofstream ofile;
    ifile.open(ifile_path);
    if (!ifile.is_open()) {
        std::cout << "Cannot find input file " << ifile_path << std::endl;
        return 1;
    }
    std::vector<std::uint8_t> v;
    if (!read_buffer_from_file(ifile, v)) {
        std::cout << "Cannot parse input file " << ifile_path << std::endl;
        return 1;
    }
    ifile.close();

    using curve_type = nil::crypto3::algebra::curves::pallas;
    using BlueprintFieldType = typename curve_type::base_field_type;
    constexpr std::size_t WitnessColumns = 15;
    constexpr std::size_t PublicInputColumns = 5;
    constexpr std::size_t ConstantColumns = 5;
    constexpr std::size_t SelectorColumns = 20;

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
    using ColumnsRotationsType = std::array<std::vector<int>, ArithmetizationParams::total_columns>;
    using ProfilingType = nil::crypto3::minimized_profiling_plonk_circuit<BlueprintFieldType, ArithmetizationParams>;

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
    iassignment.open(iassignment_path);
    if (!iassignment) {
        std::cout << "Cannot open " << iassignment_path << std::endl;
        return 1;
    }
    TableAssignmentType assignment_table;
    std::tie(table_description.usable_rows_amount, table_description.rows_amount, assignment_table) =
        load_assignment_table<BlueprintFieldType, ArithmetizationParams, ColumnType>(iassignment);
    auto columns_rotations = ProfilingType::columns_rotations(constraint_system, table_description);

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

    if (command == "gen_gate_argument") {
        print_sol_files<ProfilingType, ConstraintSystemType, ColumnsRotationsType, ArithmetizationParams>(
            constraint_system, columns_rotations, ofolder_path);
        print_params<FRIParamsType, TableDescriptionType, ColumnsRotationsType, ArithmetizationParams>(
            fri_params, table_description, columns_rotations, ofolder_path);
    }

    if (command == "gen_test_proof") {
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
            public_preprocessed_data, private_preprocessed_data, table_description, constraint_system, assignment_table,
            fri_params);

        bool verifier_res =
            nil::crypto3::zk::snark::placeholder_verifier<BlueprintFieldType, placeholder_params>::process(
                public_preprocessed_data, proof, constraint_system, fri_params);

        if (verifier_res) {
            auto filled_placeholder_proof =
                nil::crypto3::marshalling::types::fill_placeholder_proof<Endianness, ProofType>(proof);
            proof_print<Endianness, ProofType>(proof, ofolder_path + "/proof.bin");
            std::cout << "Proof is verified" << std::endl;
            iassignment.close();
            return 0;
        } else {
            std::cout << "Proof is not verified" << std::endl;
            iassignment.close();
            return 1;
        }
    }

    std::cout << "Done" << std::endl;
    return 0;
}