//---------------------------------------------------------------------------//
// Copyright (c) 2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2021 Nikita Kaskov <nbering@nil.foundation>
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------//
// @file Declaration of interfaces for PLONK unified addition component.
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_PROFILING_COMPONENT_HPP
#define CRYPTO3_PROFILING_COMPONENT_HPP

#include <fstream>
#include <sstream>
#include <filesystem>

#include <nil/crypto3/zk/snark/arithmetization/plonk/constraint_system.hpp>
#include <nil/crypto3/zk/snark/arithmetization/plonk/params.hpp>

#include <nil/blueprint/blueprint/plonk/circuit.hpp>
#include <nil/blueprint/blueprint/plonk/assignment.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/params.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/preprocessor.hpp>

namespace nil {
    namespace crypto3 {
        template<typename FieldType, typename ArithmetizationParams>
        struct minimized_profiling_plonk_circuit {
            using columns_rotations_type = std::array<std::vector<int>, ArithmetizationParams::TotalColumns>;
            using ArithmetizationType = zk::snark::plonk_constraint_system<FieldType, ArithmetizationParams>;
            using TableDescriptionType = zk::snark::plonk_table_description<FieldType, ArithmetizationParams> ;
            using GateType = zk::snark::plonk_gate<FieldType, zk::snark::plonk_constraint<FieldType>>;

            static inline columns_rotations_type
                columns_rotations(
                    ArithmetizationType &constraint_system,
                    const TableDescriptionType &table_description
                ) 
            {
                columns_rotations_type result;

                std::vector<zk::snark::plonk_gate<FieldType, zk::snark::plonk_constraint<FieldType>>> gates =
                    constraint_system.gates();

                for (std::size_t g_index = 0; g_index < gates.size(); g_index++) {

                    for (std::size_t c_index = 0; c_index < gates[g_index].constraints.size(); c_index++) {

                        for (std::size_t t_index = 0;
                                t_index < gates[g_index].constraints[c_index].terms.size();
                                t_index++) {
                            for (std::size_t v_index = 0;
                                    v_index < gates[g_index].constraints[c_index].terms[t_index].vars.size();
                                    v_index++) {

                                if (gates[g_index].constraints[c_index].terms[t_index].vars[v_index].relative) {
                                    std::size_t column_index = table_description.global_index(
                                        gates[g_index].constraints[c_index].terms[t_index].vars[v_index]);

                                    int rotation = gates[g_index]
                                                        .constraints[c_index]
                                                        .terms[t_index]
                                                        .vars[v_index]
                                                        .rotation;

                                    if (std::find(result[column_index].begin(), result[column_index].end(),
                                                    rotation) == result[column_index].end()) {
                                        result[column_index].push_back(rotation);
                                    }
                                }
                            }
                        }
                    }
                }

                std::vector<zk::snark::plonk_gate<FieldType, zk::snark::plonk_lookup_constraint<FieldType>>> lookup_gates =
                    constraint_system.lookup_gates();

                for (std::size_t g_index = 0; g_index < lookup_gates.size(); g_index++) {

                    for (std::size_t c_index = 0; c_index < lookup_gates[g_index].constraints.size();
                            c_index++) {

                        for (std::size_t v_index = 0;
                                v_index < lookup_gates[g_index].constraints[c_index].lookup_input.size();
                                v_index++) {

                            if (lookup_gates[g_index]
                                    .constraints[c_index]
                                    .lookup_input[v_index]
                                    .vars[0]
                                    .relative) {
                                std::size_t column_index = table_description.global_index(
                                    lookup_gates[g_index].constraints[c_index].lookup_input[v_index].vars[0]);

                                int rotation = lookup_gates[g_index]
                                                    .constraints[c_index]
                                                    .lookup_input[v_index]
                                                    .vars[0]
                                                    .rotation;

                                if (std::find(result[column_index].begin(), result[column_index].end(),
                                                rotation) == result[column_index].end()) {
                                    result[column_index].push_back(rotation);
                                }
                            }
                        }
                    }
                }

                for (std::size_t i = 0; i < ArithmetizationParams::TotalColumns; i++) {
                    if (std::find(result[i].begin(), result[i].end(), 0) == result[i].end()) {
                        result[i].push_back(0);
                    }
                }

                return result;
            }

            template<typename Container, typename ContainerIt>
            static bool is_last_element(const Container &c, ContainerIt it) {
                return it == (std::cend(c) - 1);
            }

            static void print_variable(std::ostream &os, const nil::crypto3::zk::snark::plonk_variable<FieldType> &var,
                                       columns_rotations_type &columns_rotations) {
                std::size_t rotation_idx =
                    std::find(std::cbegin(columns_rotations.at(var.index)),
                              std::cend(columns_rotations.at(var.index)),
                              var.rotation) -
                    std::begin(columns_rotations.at(var.index));
                os << "get_eval_i_by_rotation_idx(" << var.index << "," << rotation_idx
                   << ","
                      "mload(add(gate_params, WITNESS_EVALUATIONS_OFFSET))"
                      ")";
            }

            template<typename Vars, typename VarsIt>
            static typename std::enable_if<
                std::is_same<nil::crypto3::zk::snark::plonk_variable<FieldType>,
                             typename std::iterator_traits<typename Vars::iterator>::value_type>::value>::type
                print_term(std::ostream &os,
                           const Vars &vars,
                           VarsIt it,
                           columns_rotations_type &columns_rotations) {
                if (it != std::cend(vars)) {
                    if (!is_last_element(vars, it)) {
                        os << "mulmod(";
                    }
                    print_variable(os, *it, columns_rotations);
                    if (!is_last_element(vars, it)) {
                        os << ",";
                        print_term(os, vars, it + 1, columns_rotations);
                        os << ","
                              "modulus"
                              ")";
                    }
                }
            }

            template<typename Terms, typename TermsIt>
            static typename std::enable_if<
                std::is_same<nil::crypto3::math::non_linear_term<nil::crypto3::zk::snark::plonk_variable<FieldType>>,
                             typename std::iterator_traits<typename Terms::iterator>::value_type>::value>::type
                print_terms(std::ostream &os,
                            const Terms &terms,
                            TermsIt it,
                            columns_rotations_type &columns_rotations) {
                if (it != std::cend(terms)) {
                    os << "mstore("
                          "add(gate_params, CONSTRAINT_EVAL_OFFSET),"
                          "addmod("
                          "mload(add(gate_params, CONSTRAINT_EVAL_OFFSET)),";
                    if (it->coeff != FieldType::value_type::one()) {
                        if (it->vars.size()) {
                            os << "mulmod(0x" << std::hex << it->coeff.data << std::dec << ",";
                        } else {
                            os << "0x" << std::hex << it->coeff.data << std::dec;
                        }
                    }
                    print_term(os, it->vars, std::cbegin(it->vars), columns_rotations);
                    if (it->coeff != FieldType::value_type::one()) {
                        if (it->vars.size()) {
                            os << ","
                                  "modulus"
                                  ")";
                        }
                    }
                    os << ","
                          "modulus"
                          "))"
                       << std::endl;
                    print_terms(os, terms, it + 1, columns_rotations);
                }
            }

            static void
                print_constraint(std::ostream &os,
                                 const typename nil::crypto3::zk::snark::plonk_constraint<FieldType> &constraint,
                                 columns_rotations_type &columns_rotations) {
                os << "mstore(add(gate_params, CONSTRAINT_EVAL_OFFSET), 0)" << std::endl;
                print_terms(os, constraint.terms, std::cbegin(constraint.terms), columns_rotations);
            }

            static void print_gate_evaluation(std::ostream &os) {
                os << "mstore("
                      "add(gate_params, GATE_EVAL_OFFSET),"
                      "addmod("
                      "mload(add(gate_params, GATE_EVAL_OFFSET)),"
                      "mulmod("
                      "mload(add(gate_params, CONSTRAINT_EVAL_OFFSET)),"
                      "theta_acc,"
                      "modulus"
                      "),"
                      "modulus"
                      ")"
                      ")"
                   << std::endl;
            }

            static void print_theta_acc(std::ostream &os) {
                os << "theta_acc := mulmod("
                      "theta_acc,"
                      "mload(add(gate_params, THETA_OFFSET)),"
                      "modulus"
                      ")"
                   << std::endl;
            }

            static void print_selector(std::ostream &os,
                                       const nil::crypto3::zk::snark::plonk_gate<
                                           FieldType, nil::crypto3::zk::snark::plonk_constraint<FieldType>> &gate) {
                os << "mstore("
                      "add(gate_params, GATE_EVAL_OFFSET),"
                      "mulmod("
                      "mload(add(gate_params, GATE_EVAL_OFFSET)),"
                      "get_selector_i("
                   << gate.selector_index
                   << ","
                      "mload(add(gate_params, SELECTOR_EVALUATIONS_OFFSET))"
                      "),"
                      "modulus"
                      ")"
                      ")"
                   << std::endl;
            }

            static void print_argument_evaluation(std::ostream &os) {
                os << "gates_evaluation := addmod("
                      "gates_evaluation,"
                      "mload(add(gate_params, GATE_EVAL_OFFSET)),"
                      "modulus"
                      ")"
                   << std::endl;
            }

            static void print_gate(std::ostream &os,
                                   const nil::crypto3::zk::snark::plonk_gate<
                                       FieldType, nil::crypto3::zk::snark::plonk_constraint<FieldType>> &gate,
                                   columns_rotations_type &columns_rotations) {
                os << "mstore(add(gate_params, GATE_EVAL_OFFSET), 0)" << std::endl;
                for (auto &constraint : gate.constraints) {
                    print_constraint(os, constraint, columns_rotations);
                    print_gate_evaluation(os);
                    print_theta_acc(os);
                }
                print_selector(os, gate);
                print_argument_evaluation(os);
            }

            static void process(std::ostream &os, ArithmetizationType &bp,
                                columns_rotations_type &columns_rotations) {
                for (const auto &gate : bp.gates()) {
                    print_gate(os, gate, columns_rotations);
                }
            }

            static void print_gate_file(int gate_ind, std::ostream &gate_out, const GateType &gate, columns_rotations_type &columns_rotations){
                gate_out << 
"pragma solidity >=0.8.4;" << std::endl <<
std::endl <<
"import \"../contracts/types.sol\";" << std::endl<<
"import \"../contracts/logging.sol\";"<< std::endl <<
"import \"../contracts/basic_marshalling.sol\";"<< std::endl <<

"library gate"<< gate_ind << " {"<< std::endl <<
"    uint256 constant MODULUS_OFFSET = 0x0;"<< std::endl <<
"    uint256 constant THETA_OFFSET = 0x20;"<< std::endl <<
"    uint256 constant CONSTRAINT_EVAL_OFFSET = 0x40;"<< std::endl <<
"    uint256 constant GATE_EVAL_OFFSET = 0x60;"<< std::endl <<
"    uint256 constant WITNESS_EVALUATIONS_OFFSET_OFFSET = 0x80;"<< std::endl <<
"    uint256 constant SELECTOR_EVALUATIONS_OFFSET = 0xa0;"<< std::endl <<
"    uint256 constant EVAL_PROOF_WITNESS_OFFSET_OFFSET = 0xc0;"<< std::endl <<
"    uint256 constant EVAL_PROOF_SELECTOR_OFFSET_OFFSET = 0xe0;"<< std::endl <<
"    uint256 constant GATES_EVALUATION_OFFSET = 0x100;"<< std::endl <<
"    uint256 constant THETA_ACC_OFFSET = 0x120;"<< std::endl <<
"    uint256 constant SELECTOR_EVALUATIONS_OFFSET_OFFSET = 0x140;"<< std::endl <<
"    uint256 constant OFFSET_OFFSET = 0x160;"<< std::endl <<
"    uint256 constant WITNESS_EVALUATIONS_OFFSET = 0x180;"<< std::endl <<
"    uint256 constant CONSTANT_EVALUATIONS_OFFSET = 0x1a0;"<< std::endl <<
"    uint256 constant PUBLIC_INPUT_EVALUATIONS_OFFSET = 0x1c0;"<< std::endl <<
std::endl <<
std::endl <<
"    function evaluate_gate_be("<< std::endl <<
"        types.gate_argument_local_vars memory gate_params,"<< std::endl <<
"        int256[][] memory columns_rotations"<< std::endl <<
"    ) external pure returns (uint256 gates_evaluation, uint256 theta_acc) {"<< std::endl <<
"        gates_evaluation = gate_params.gates_evaluation;"<< std::endl <<
"        theta_acc = gate_params.theta_acc;"<< std::endl <<
std::endl <<
"    assembly {"<< std::endl <<
"            let modulus := mload(gate_params)"<< std::endl <<
"            mstore(add(gate_params, GATE_EVAL_OFFSET), 0)"<< std::endl <<
std::endl <<
"            function get_eval_i_by_rotation_idx(idx, rot_idx, ptr) -> result {"<< std::endl <<
"                result := mload(add(add(mload(add(add(ptr, 0x20), mul(0x20, idx))), 0x20),"<< std::endl <<
"                          mul(0x20, rot_idx)))"<< std::endl <<
"            }"<< std::endl <<
std::endl <<
"            function get_selector_i(idx, ptr) -> result {"<< std::endl <<
"                result := mload(add(add(ptr, 0x20), mul(0x20, idx)))"<< std::endl <<
"            }" << std::endl;
            // TODO: insert generated code for gate argument evaluation here

                print_gate(gate_out, gate, columns_rotations);
                gate_out << "}}}"<< std::endl;
            }

            static void print_main_file(std::ostream &out, 
                ArithmetizationType &bp,
                columns_rotations_type &columns_rotations) 
            {
                out <<
"pragma solidity >=0.8.4;" << std::endl << 
std::endl <<
"import \"../contracts/types.sol\";" << std::endl <<
"import \"../contracts/basic_marshalling.sol\";" << std::endl <<
"import \"../contracts/commitments/batched_lpc_verifier.sol\";" << std:: endl;
                // ind???
                for (std::size_t ind = 0; ind < bp.gates().size(); ind++) {
                    out << "import \"./gate"<< ind <<".sol\";"<<std::endl;  
                }              

                out << std::endl <<
"contract gate_argument_split_gen {" << std::endl <<
"    uint256 constant WITNESSES_N = " << ArithmetizationParams::WitnessColumns << ";" << std::endl <<
"    uint256 constant SELECTOR_N = " << ArithmetizationParams::SelectorColumns << ";" << std::endl <<
"    uint256 constant PUBLIC_INPUT_N =" << ArithmetizationParams::PublicInputColumns<< ";" << std::endl <<
"    uint256 constant GATES_N = " << bp.gates().size() << ";" << std::endl <<
"    uint256 constant CONSTANTS_N = " << ArithmetizationParams::ConstantColumns << ";" << std::endl <<
std::endl <<
"    function evaluate_gates_be(" << std::endl <<
"        bytes calldata blob," << std::endl <<
"        types.gate_argument_local_vars memory gate_params," << std::endl <<
"        types.arithmetization_params memory ar_params," << std::endl <<
"        int256[][] memory columns_rotations" << std::endl <<
"    ) external view returns (uint256 gates_evaluation) {" << std::endl <<
std::endl <<
"        gate_params.witness_evaluations = new uint256[][](WITNESSES_N);" << std::endl <<
"        gate_params.offset = batched_lpc_verifier.skip_to_z(blob,  gate_params.eval_proof_witness_offset);" << std::endl <<
"        for (uint256 i = 0; i < WITNESSES_N; i++) {" << std::endl <<
"            gate_params.witness_evaluations[i] = new uint256[](columns_rotations[i].length);" << std::endl <<
"            for (uint256 j = 0; j < columns_rotations[i].length; j++) {" << std::endl <<
"                gate_params.witness_evaluations[i][j] = basic_marshalling.get_i_j_uint256_from_vector_of_vectors(blob, gate_params.offset, i, j);" << std::endl <<
"            }" << std::endl <<
"        }" << std::endl <<
std::endl <<
"        gate_params.selector_evaluations = new uint256[](GATES_N);" << std::endl <<
"        gate_params.offset = batched_lpc_verifier.skip_to_z(blob,  gate_params.eval_proof_selector_offset);" << std::endl <<
"        for (uint256 i = 0; i < GATES_N; i++) {" << std::endl <<
"           gate_params.selector_evaluations[i] = basic_marshalling.get_i_j_uint256_from_vector_of_vectors(" << std::endl <<
"                blob, " << std::endl <<
"                gate_params.offset, " << std::endl <<
"                i + ar_params.permutation_columns + ar_params.permutation_columns + ar_params.constant_columns, " << std::endl <<
"                0" << std::endl <<
"            );" << std::endl <<
"        }" << std::endl <<
std::endl <<
"        gate_params.constant_evaluations = new uint256[][](CONSTANTS_N);" << std::endl <<
"        gate_params.offset = batched_lpc_verifier.skip_to_z(blob,  gate_params.eval_proof_constant_offset);" << std::endl <<
"        for (uint256 i = 0; i < CONSTANTS_N; i++) {" << std::endl <<
"            gate_params.constant_evaluations[i] = new uint256[](columns_rotations[i].length);" << std::endl <<
"            for (uint256 j = 0; j < columns_rotations[i].length; j++) {" << std::endl <<
"                gate_params.constant_evaluations[i][j] = basic_marshalling.get_i_j_uint256_from_vector_of_vectors(" << std::endl <<
"                    blob, " << std::endl <<
"                    gate_params.offset, " << std::endl <<
"                    i + ar_params.permutation_columns + ar_params.permutation_columns, " << std::endl <<
"                    j" << std::endl <<
"                );" << std::endl <<
"            }" << std::endl <<
"        }" << std::endl <<
std::endl <<
"        gate_params.theta_acc = 1;" << std::endl <<
"        gate_params.gates_evaluation = 0;" << std::endl;

                for (std::size_t ind = 0; ind < bp.gates().size(); ind++) {
                    out << 
"        (gate_params.gates_evaluation, gate_params.theta_acc) = gate" << ind <<  ".evaluate_gate_be(gate_params, columns_rotations);" << std::endl;
                }
                out <<
"        gates_evaluation = gate_params.gates_evaluation;}}" << std::endl;
            }

            static void print_linked_libraries_list(std::ostream &out, std::size_t lib_num){
                bool first = true;
                out << "[";
                for( size_t i = 0; i < lib_num; i++){
                    if( first ) 
                        first = false;
                    else
                        out << "," << std::endl;
                    out << "\"gate" <<i << "\""; 
                }
                out << std::endl << "]" << std::endl;
            }

            static void process_split(ArithmetizationType &bp,
                columns_rotations_type &columns_rotations, 
                std::string out_folder_path = ".") 
            {
                std::ofstream gate_argument_out;
                gate_argument_out.open(out_folder_path + "/gate_argument.sol");
                print_main_file(gate_argument_out, bp, columns_rotations);
                gate_argument_out.close();

                std::ofstream json_out;
                json_out.open(out_folder_path + "/linked_libs_list.json");
                print_linked_libraries_list(json_out, bp.gates().size());
                json_out.close();

                for (const auto &gate : bp.gates()) {
                    std::ofstream gate_out;
                    gate_out.open(out_folder_path + "/gate" + std::to_string(gate.selector_index) + ".sol");
                    print_gate_file(gate.selector_index, gate_out, gate, columns_rotations);
                    gate_out.close();
                }
            }
        };
    }    // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_PROFILING_COMPONENT_HPP
