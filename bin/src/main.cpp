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

#include <nil/crypto3/algebra/curves/pallas.hpp>

#include <nil/crypto3/zk/snark/arithmetization/plonk/params.hpp>

#include <nil/crypto3/zk/blueprint/plonk/assignment.hpp>
#include <nil/crypto3/zk/blueprint/plonk/circuit.hpp>
#include <nil/crypto3/zk/components/algebra/fields/plonk/addition.hpp>
#include <nil/crypto3/zk/components/algebra/fields/plonk/subtraction.hpp>
#include <nil/crypto3/zk/components/algebra/fields/plonk/multiplication.hpp>
#include <nil/crypto3/zk/components/algebra/fields/plonk/division.hpp>
#include <nil/crypto3/zk/components/algebra/fields/plonk/multiplication_by_constant.hpp>
#include <nil/crypto3/zk/components/algebra/fields/plonk/division_or_zero.hpp>
#include <nil/crypto3/zk/components/hashes/poseidon/plonk/poseidon_15_wires.hpp>
#include <nil/crypto3/zk/utils/table_profiling.hpp>
#include <nil/crypto3/zk/utils/satisfiability_check.hpp>

using namespace nil;
using namespace nil::crypto3;

struct BlueprintInstr {
    enum opcode_type {
        FADD,
        FSUB,
        FMUL,
        FDIV,
        POSEIDON
    } opcode;
    std::vector<std::string> arguments;
};

template<typename BlueprintFieldType,
         typename ArithmetizationParams>
struct parser {
    
    using ArithmetizationType = zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;
    using var = zk::snark::plonk_variable<BlueprintFieldType>;

    blueprint::circuit<ArithmetizationType> bp;
    blueprint::assignment<ArithmetizationType> assignment;

    void parse_instruction(std::map<std::string, var> &variables, const BlueprintInstr& instruction){
        
        std::size_t start_row = assignment.allocated_rows();

        switch (instruction.opcode){
            case BlueprintInstr::opcode_type::FADD:
                {
                    using component_type = blueprint::components::addition<ArithmetizationType, 3>;

                    var x = variables[instruction.arguments[0]];
                    var y = variables[instruction.arguments[1]];

                    typename component_type::input_type instance_input = {x,y};

                    component_type component_instance({0, 1, 2},{},{});

                    blueprint::components::generate_circuit<BlueprintFieldType, ArithmetizationParams>(
                        component_instance, bp, assignment, instance_input, start_row);
                    typename component_type::result_type component_result =
                        blueprint::components::generate_assignments<BlueprintFieldType, ArithmetizationParams>(
                            component_instance, assignment, instance_input, start_row);

                    variables[instruction.arguments[2]] = component_result.output;

                    break;
                }
            case BlueprintInstr::opcode_type::FSUB:
                {
                    using component_type = blueprint::components::subtraction<ArithmetizationType, 3>;

                    var x = variables[instruction.arguments[0]];
                    var y = variables[instruction.arguments[1]];

                    typename component_type::input_type instance_input = {x,y};

                    component_type component_instance({0, 1, 2},{},{});

                    blueprint::components::generate_circuit<BlueprintFieldType, ArithmetizationParams>(
                        component_instance, bp, assignment, instance_input, start_row);
                    typename component_type::result_type component_result =
                        blueprint::components::generate_assignments<BlueprintFieldType, ArithmetizationParams>(
                            component_instance, assignment, instance_input, start_row);

                    variables[instruction.arguments[2]] = component_result.output;
                    break;
                }
            case BlueprintInstr::opcode_type::FMUL:
                {
                    using component_type = blueprint::components::multiplication<ArithmetizationType, 3>;

                    var x = variables[instruction.arguments[0]];
                    var y = variables[instruction.arguments[1]];

                    typename component_type::input_type instance_input = {x,y};

                    component_type component_instance({0, 1, 2},{},{});

                    blueprint::components::generate_circuit<BlueprintFieldType, ArithmetizationParams>(
                        component_instance, bp, assignment, instance_input, start_row);
                    typename component_type::result_type component_result =
                        blueprint::components::generate_assignments<BlueprintFieldType, ArithmetizationParams>(
                            component_instance, assignment, instance_input, start_row);

                    variables[instruction.arguments[2]] = component_result.output;
                    break;
                }
            case BlueprintInstr::opcode_type::FDIV:
                {
                    using component_type = blueprint::components::division<ArithmetizationType, 4>;

                    var x = variables[instruction.arguments[0]];
                    var y = variables[instruction.arguments[1]];

                    typename component_type::input_type instance_input = {x,y};

                    component_type component_instance({0, 1, 2, 3},{},{});

                    blueprint::components::generate_circuit<BlueprintFieldType, ArithmetizationParams>(
                        component_instance, bp, assignment, instance_input, start_row);
                    typename component_type::result_type component_result =
                        blueprint::components::generate_assignments<BlueprintFieldType, ArithmetizationParams>(
                            component_instance, assignment, instance_input, start_row);

                    variables[instruction.arguments[2]] = component_result.output;
                    break;
                }
            case BlueprintInstr::opcode_type::POSEIDON:
                {
                    using component_type = blueprint::components::poseidon<ArithmetizationType, BlueprintFieldType, 15>;

                    std::array<var, component_type::state_size> input_state_var;
                    for (std::uint32_t i = 0; i < component_type::state_size; i++){
                        input_state_var[i] = variables[instruction.arguments[i]];
                    }

                    typename component_type::input_type instance_input = {input_state_var};

                    component_type component_instance({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},{},{});

                    blueprint::components::generate_circuit<BlueprintFieldType, ArithmetizationParams>(
                        component_instance, bp, assignment, instance_input, start_row);

                    typename component_type::result_type component_result =
                        blueprint::components::generate_assignments<BlueprintFieldType, ArithmetizationParams>(
                            component_instance, assignment, instance_input, start_row);

                    for (std::uint32_t i = 0; i < component_type::state_size; i++){
                        variables[instruction.arguments[component_type::state_size + i]] = component_result.output_state[i];
                    }
                    break;
                }
            default:
                assert(1 == 0 && "unsupported opcode type");
        }
    }

    template <typename PublicInputContainerType>
    void evaluate (std::vector<BlueprintInstr> code, const PublicInputContainerType &public_input){

        std::map<std::string, var> variables;

        for (std::size_t i = 0; i < public_input.size(); i++) {
            assignment.public_input(0, i) = (public_input[i]);
            variables[code[0].arguments[i]] = var(0, i, false, var::column_type::public_input);
        }

        for (std::int32_t instruction_index = 0; instruction_index < code.size(); instruction_index++){
            parse_instruction(variables, code[instruction_index]);
        }
    }
};

int main (){
    using curve_type = algebra::curves::pallas;
    using BlueprintFieldType = typename curve_type::base_field_type;
    constexpr std::size_t WitnessColumns = 15;
    constexpr std::size_t PublicInputColumns = 5;
    constexpr std::size_t ConstantColumns = 5;
    constexpr std::size_t SelectorColumns = 20;

    using ArithmetizationParams =
        zk::snark::plonk_arithmetization_params<WitnessColumns, PublicInputColumns, ConstantColumns, SelectorColumns>;

    std::vector<BlueprintInstr> code = {
        {BlueprintInstr::POSEIDON, {"v0", "v1", "v2", "v3", "v4", "v5"}},
        {BlueprintInstr::FADD, {"v0", "v1", "v8"}},
        {BlueprintInstr::FMUL, {"v0", "v1", "v3"}},
        {BlueprintInstr::FMUL, {"v0", "v2", "v4"}},
        {BlueprintInstr::FADD, {"v1", "v3", "v5"}},
        {BlueprintInstr::FMUL, {"v4", "v5", "v6"}}
    };

    std::vector<typename BlueprintFieldType::value_type> public_input = {0, 1, 1};
    parser<BlueprintFieldType, ArithmetizationParams> parser_instance;

    parser_instance.evaluate(code, public_input);

    zk::snark::plonk_table_description<BlueprintFieldType, ArithmetizationParams> desc;
    desc.usable_rows_amount = parser_instance.assignment.rows_amount();
    desc.rows_amount = zk::snark::basic_padding(parser_instance.assignment);
    std::cout << "Usable rows: " << desc.usable_rows_amount << std::endl;
    std::cout << "Padded rows: " << desc.rows_amount << std::endl;

    blueprint::profiling(parser_instance.assignment);

    assert(blueprint::is_satisfied(parser_instance.bp, parser_instance.assignment) && "circuit satisfiability check failed");
}