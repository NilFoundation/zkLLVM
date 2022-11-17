#include <iostream>
#include <fstream>
#include <vector>

#include <nil/crypto3/algebra/curves/pallas.hpp>

#include <nil/crypto3/zk/snark/arithmetization/plonk/params.hpp>
#include <nil/crypto3/zk/snark/arithmetization/plonk/constraint_system.hpp>

#include <nil/marshalling/status_type.hpp>
#include <nil/marshalling/field_type.hpp>
#include <nil/marshalling/endianness.hpp>
#include <nil/crypto3/marshalling/zk/types/plonk/constraint_system.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/preprocessor.hpp>

#include <nil/blueprint/component.hpp>
#include <nil/blueprint/parser.hpp>
#include "minimized_profiling_plonk_circuit.hpp"

bool read_buffer_from_file(std::ifstream &ifile, std::vector<std::uint8_t> &v){
    char c;
    char c1;
    uint8_t b;

    ifile >> c;
    if( c != '0' ) return false;
    ifile >> c;
    if( c != 'x' ) return false;
    while(ifile){
        std::string str = "";
        ifile >> c >> c1;
        if(!isxdigit(c) || !isxdigit(c1)) return false;
        str += c;
        str += c1;
        b = stoi(str, 0, 0x10);
        v.push_back(b);
    }
    return true;
}

template<typename ProfilingType, typename ConstraintSystemType, typename ColumnsRotationsType, typename ArithmetizationParams>
void print_sol_file(std::ostream &os, ConstraintSystemType &constraint_system, ColumnsRotationsType &columns_rotations){
    os <<
"pragma solidity >=0.8.4;\n\n" <<
"import \"../types.sol\";\n" <<
"import \"../basic_marshalling.sol\";\n"<<
"import \"../commitments/batched_lpc_verifier.sol\";\n"<<
"\n"<<
"contract gate_argument_gen {\n"<<
"    uint256 constant WITNESSES_N = "<< ArithmetizationParams::WitnessColumns <<";\n"<<
"    uint256 constant GATES_N = "<< constraint_system.gates().size() <<";\n"<<
"\n" <<
"    uint256 constant MODULUS_OFFSET = 0x0;\n"<<
"    uint256 constant THETA_OFFSET = 0x20;\n"<<
"    uint256 constant CONSTRAINT_EVAL_OFFSET = 0x40;\n"<<
"    uint256 constant GATE_EVAL_OFFSET = 0x60;\n"<<
"    uint256 constant WITNESS_EVALUATIONS_OFFSET = 0x80;\n"<<
"    uint256 constant SELECTOR_EVALUATIONS_OFFSET = 0xa0;\n"<<
"    uint256 constant EVAL_PROOF_WITNESS_OFFSET_OFFSET = 0xc0;\n"<<
"    uint256 constant EVAL_PROOF_SELECTOR_OFFSET_OFFSET = 0xe0;\n"<<
"    uint256 constant GATES_EVALUATION_OFFSET = 0x100;\n"<<
"    uint256 constant THETA_ACC_OFFSET = 0x120;\n"<<
"    uint256 constant SELECTOR_EVALUATIONS_OFFSET_OFFSET = 0x140;\n"<<
"    uint256 constant OFFSET_OFFSET = 0x160;\n"<<
"\n"<<
"    function evaluate_gates_be(\n"<<
"        bytes calldata blob,\n"<<
"        types.gate_argument_local_vars memory gate_params,\n"<<
"        types.arithmetization_params memory ar_params,\n"<<
"        int256[][] memory columns_rotations\n"<<
"    ) external pure returns (uint256 gates_evaluation) {\n"<<
"\n"<<
"        gate_params.offset = basic_marshalling.skip_length(\n"<<
"            batched_lpc_verifier.skip_to_z(\n"<<
"                blob,\n"<<
"                gate_params.eval_proof_witness_offset\n"<<
"            )\n"<<
"        );\n"<<
"        gate_params.witness_evaluations_offsets = new uint256[](WITNESSES_N);\n"<<
"        for (uint256 i = 0; i < WITNESSES_N; i++) {\n"<<
"            gate_params.witness_evaluations_offsets[i] = basic_marshalling\n"<<
"                .get_i_uint256_ptr_from_vector(blob, gate_params.offset, 0);\n"<<
"            gate_params.offset = basic_marshalling.skip_vector_of_uint256_be(\n"<<
"                blob,\n"<<
"                gate_params.offset\n"<<
"            );\n"<<
"        }\n"<<
"        gate_params.selector_evaluations = new uint256[](GATES_N);\n"<<
"        for (uint256 i = 0; i < GATES_N; i++) {\n"<<
"            gate_params.selector_evaluations[i] = batched_lpc_verifier\n"<<
"                .get_z_i_j_from_proof_be(\n"<<
"                    blob,\n"<<
"                    gate_params.eval_proof_selector_offset,\n"<<
"                    i + ar_params.permutation_columns + ar_params.permutation_columns + ar_params.constant_columns,\n"<<
"                    0\n"<<
"                );\n"<<
"        }\n"<<
"\n"<<
"        uint256 t = 0;\n"<<
"        assembly {\n"<<
"            let modulus := mload(gate_params)\n"<<
"            let theta_acc := 1\n"<<
"            function get_eval_i_by_rotation_idx(idx, rot_idx, ptr) -> result {" <<
"                result := calldataload(\n"<<
"                    add(\n"<<
"                        mload(add(add(ptr, 0x20), mul(0x20, idx))),\n"<<
"                        mul(0x20, rot_idx)\n"<<
"                    )\n"<<
"                )\n"<<
"            }\n"<<
"\n"<<
"            function get_selector_i(idx, ptr) -> result {\n"<<
"                result := mload(add(add(ptr, 0x20), mul(0x20, idx)))\n"<<
"            }\n"<<
"            let x1 := add(gate_params, CONSTRAINT_EVAL_OFFSET)\n"<<
"            let x2 := add(gate_params, WITNESS_EVALUATIONS_OFFSET)\n"<<
"            let x3 := get_eval_i_by_rotation_idx(0,0,mload(x2))\n"<<
"            let x4 := get_eval_i_by_rotation_idx(2,0,mload(x2))\n";

    ProfilingType::process(os, constraint_system, columns_rotations);
    os << "}\n}\n}" << std:: endl;
}

template<typename ProfilingType, typename ConstraintSystemType, typename ColumnsRotationsType, typename ArithmetizationParams>
void print_sol_files(ConstraintSystemType &constraint_system, ColumnsRotationsType &columns_rotations, std::string out_folder_path = "."){
    ProfilingType::process_split(constraint_system, columns_rotations, out_folder_path);
}

int main(int argc, char *argv[]) {
    if(argc < 2){
        std::cout << "First parameter is path to the file with plonk_constraint_system marshalling" << std::endl;
        return 1;
    }
    if(argc > 3){
        std::cout << "This program is not so complex yet" << std::endl;
        return 1;
    }

    std::ifstream ifile;
    std::ofstream ofile;
    ifile.open(argv[1]);
    if( !ifile.is_open() ){
        std::cout << "Cannot find input file " << argv[1] << std::endl;
        return 1;
    }
    std::vector<std::uint8_t> v;
    if( !read_buffer_from_file(ifile, v)){
        std::cout << "Cannot parse input file "<< argv[1] << std::endl;
        return 1;
    }
    ifile.close();

    using curve_type = nil::crypto3::algebra::curves::pallas;
    using BlueprintFieldType = typename curve_type::base_field_type;
    constexpr std::size_t WitnessColumns = 15;
    constexpr std::size_t PublicInputColumns = 5;
    constexpr std::size_t ConstantColumns = 5;
    constexpr std::size_t SelectorColumns = 20;

    using ArithmetizationParams = nil::crypto3::zk::snark::plonk_arithmetization_params<WitnessColumns, PublicInputColumns, ConstantColumns, SelectorColumns>;
    using ConstraintSystemType = nil::crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;
    using TableDescriptionType = nil::crypto3::zk::snark::plonk_table_description<BlueprintFieldType, ArithmetizationParams>;
    using Endianness = nil::marshalling::option::big_endian;
    using TTypeBase = nil::marshalling::field_type<Endianness>;
    using value_marshalling_type = nil::crypto3::marshalling::types::plonk_constraint_system<TTypeBase, ConstraintSystemType>;
    using ColumnsRotationsType = std::array<std::vector<int>, ArithmetizationParams::TotalColumns>;
    using ProfilingType = nil::crypto3::minimized_profiling_plonk_circuit<BlueprintFieldType, ArithmetizationParams>;
    
    value_marshalling_type  marshalled_data;
    TableDescriptionType    table_description;
    auto read_iter = v.begin();
    auto status = marshalled_data.read(read_iter, v.size());
    auto constraint_system = nil::crypto3::marshalling::types::make_plonk_constraint_system<ConstraintSystemType, Endianness>(marshalled_data);
    auto columns_rotations = ProfilingType::columns_rotations(constraint_system, table_description);

    if(argc == 2 ){
        print_sol_files<ProfilingType, ConstraintSystemType, ColumnsRotationsType, ArithmetizationParams>(constraint_system, columns_rotations);
    } else {
        print_sol_files<ProfilingType, ConstraintSystemType, ColumnsRotationsType, ArithmetizationParams>(constraint_system, columns_rotations, argv[2]);
    }
    std::cout << "Done" << std::endl;
    return 0;
}