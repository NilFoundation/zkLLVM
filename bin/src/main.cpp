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
#include <nil/crypto3/zk/components/algebra/fields/plonk/multiplication.hpp>
#include <nil/crypto3/zk/components/algebra/fields/plonk/addition.hpp>
#include <nil/crypto3/zk/components/algebra/fields/plonk/division.hpp>
#include <nil/crypto3/zk/components/algebra/fields/plonk/subtraction.hpp>
#include <nil/crypto3/zk/components/algebra/fields/plonk/multiplication_by_constant.hpp>
#include <nil/crypto3/zk/components/algebra/fields/plonk/division_or_zero.hpp>

using namespace nil;
using namespace nil::crypto3;

struct BlueprintInstr {
    enum opcode_type {
        FADD,
        FSUB,
        FMUL,
        FDIV,
    } opcode;
    std::vector<std::string> arguments;
};

template<typename BlueprintFieldType,
         typename ArithmetizationParams>
struct parser {
    
    using ArithmetizationType = zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;
    using var = zk::snark::plonk_variable<BlueprintFieldType>;

    blueprint::circuit<ArithmetizationType> bp;
    blueprint::private_assignment<ArithmetizationType> private_assignment;
    blueprint::public_assignment<ArithmetizationType> public_assignment;
    std::size_t start_row = 0;

    void parse_instruction(std::map<std::string, var> &variables, const BlueprintInstr& instruction){
        switch (instruction.opcode){
            case BlueprintInstr::opcode_type::FADD:
                {
                    using component_type = blueprint::components::addition<ArithmetizationType, 3>;

                    var x = variables[instruction.arguments[0]];
                    var y = variables[instruction.arguments[1]];

                    typename component_type::input_type instance_input = {x,y};

                    component_type component_instance({0, 1, 2},{},{});

                    blueprint::components::generate_circuit<BlueprintFieldType, ArithmetizationParams>(
                        component_instance, bp, public_assignment, instance_input, start_row);
                    typename component_type::result_type component_result =
                        blueprint::components::generate_assignments<BlueprintFieldType, ArithmetizationParams>(
                            component_instance, private_assignment, public_assignment, instance_input, start_row);

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
                        component_instance, bp, public_assignment, instance_input, start_row);
                    typename component_type::result_type component_result =
                        blueprint::components::generate_assignments<BlueprintFieldType, ArithmetizationParams>(
                            component_instance, private_assignment, public_assignment, instance_input, start_row);

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
                        component_instance, bp, public_assignment, instance_input, start_row);
                    typename component_type::result_type component_result =
                        blueprint::components::generate_assignments<BlueprintFieldType, ArithmetizationParams>(
                            component_instance, private_assignment, public_assignment, instance_input, start_row);

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
                        component_instance, bp, public_assignment, instance_input, start_row);
                    typename component_type::result_type component_result =
                        blueprint::components::generate_assignments<BlueprintFieldType, ArithmetizationParams>(
                            component_instance, private_assignment, public_assignment, instance_input, start_row);

                    variables[instruction.arguments[2]] = component_result.output;
                    break;
                }
            default:
                assert(1 == 0 && "unsupported opcode type");
        }
    }

    template <typename PublicInputContainerType>
    void evaluate (std::vector<BlueprintInstr> code, const PublicInputContainerType &public_input){
        std::size_t start_row = 0;

        std::map<std::string, var> variables;

        for (std::size_t i = 0; i < public_input.size(); i++) {
            public_assignment.public_input(0, start_row + i) = (public_input[i]);
            variables[code[0].arguments[i]] = var(0, start_row + i, false, var::column_type::public_input);
        }

        for (std::int32_t instruction_index = 1; instruction_index < code.size(); instruction_index++){
            parse_instruction(variables, code[instruction_index]);
        }
    }
};

int main (){
    using curve_type = algebra::curves::pallas;
    using BlueprintFieldType = typename curve_type::base_field_type;
    constexpr std::size_t WitnessColumns = 3;
    constexpr std::size_t PublicInputColumns = 1;
    constexpr std::size_t ConstantColumns = 0;
    constexpr std::size_t SelectorColumns = 1;

    using ArithmetizationParams =
        zk::snark::plonk_arithmetization_params<WitnessColumns, PublicInputColumns, ConstantColumns, SelectorColumns>;

    std::vector<BlueprintInstr> code = {{BlueprintInstr::FADD, {"v0", "v1", "v2"}}};

    std::vector<typename BlueprintFieldType::value_type> public_input = {2, 12};
    parser<BlueprintFieldType, ArithmetizationParams> parser_instance;
    parser_instance.evaluate(code, public_input);
}