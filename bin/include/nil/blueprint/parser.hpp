//---------------------------------------------------------------------------//
// Copyright (c) 2022 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2022 Nikita Kaskov <nbering@nil.foundation>
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

#ifndef CRYPTO3_BLUEPRINT_COMPONENT_INSTRUCTION_PARSER_HPP
#define CRYPTO3_BLUEPRINT_COMPONENT_INSTRUCTION_PARSER_HPP

#include <nil/blueprint/blueprint/plonk/assignment.hpp>
#include <nil/blueprint/blueprint/plonk/circuit.hpp>
#include <nil/blueprint/components/algebra/fields/plonk/addition.hpp>
#include <nil/blueprint/components/algebra/fields/plonk/subtraction.hpp>
#include <nil/blueprint/components/algebra/fields/plonk/multiplication.hpp>
#include <nil/blueprint/components/algebra/fields/plonk/division.hpp>
#include <nil/blueprint/components/algebra/fields/plonk/multiplication_by_constant.hpp>
#include <nil/blueprint/components/algebra/fields/plonk/division_or_zero.hpp>
#include <nil/blueprint/components/hashes/poseidon/plonk/poseidon_15_wires.hpp>

namespace nil {
    namespace blueprint {

        struct BlueprintInstr {
            enum opcode_type { FADD, FSUB, FMUL, FDIV, POSEIDON } opcode;
            std::vector<std::string> arguments;

            static const char *opcode_to_str(opcode_type opcode) {
                switch (opcode) {
#define OPCODE_CASE(Opcode) \
    case Opcode:            \
        return #Opcode

                    OPCODE_CASE(FADD);
                    OPCODE_CASE(FSUB);
                    OPCODE_CASE(FMUL);
                    OPCODE_CASE(FDIV);
                    OPCODE_CASE(POSEIDON);
#undef OPCODE_CASE
                    default:
                        std::cerr << "Unknown BlueprintInstr opcode";
                        std::abort();
                }
            }
        };

        struct BlueprintCode {
            int input_size;
            std::vector<BlueprintInstr> instructions;

            void dump() {
                for (const auto &inst : instructions) {
                    std::cout << BlueprintInstr::opcode_to_str(inst.opcode) << ' ';
                    for (const auto &reg : inst.arguments) {
                        std::cout << reg << ' ';
                    }
                    std::cout << std::endl;
                }
            }
        };

        template<typename BlueprintFieldType, typename ArithmetizationParams>
        struct parser {

            using ArithmetizationType =
                crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;
            using var = crypto3::zk::snark::plonk_variable<BlueprintFieldType>;

            circuit<ArithmetizationType> bp;
            assignment<ArithmetizationType> assignmnt;

        private:
            void parse_instruction(std::map<std::string, var> &variables, const BlueprintInstr &instruction) {

                std::size_t start_row = assignmnt.allocated_rows();

                switch (instruction.opcode) {
                    case BlueprintInstr::opcode_type::FADD: {
                        using component_type = components::addition<ArithmetizationType, 3>;

                        var x = variables[instruction.arguments[0]];
                        var y = variables[instruction.arguments[1]];

                        typename component_type::input_type instance_input = {x, y};

                        component_type component_instance({0, 1, 2}, {}, {});

                        components::generate_circuit<BlueprintFieldType, ArithmetizationParams>(
                            component_instance, bp, assignmnt, instance_input, start_row);
                        typename component_type::result_type component_result =
                            components::generate_assignments<BlueprintFieldType, ArithmetizationParams>(
                                component_instance, assignmnt, instance_input, start_row);

                        variables[instruction.arguments[2]] = component_result.output;

                        break;
                    }
                    case BlueprintInstr::opcode_type::FSUB: {
                        using component_type = components::subtraction<ArithmetizationType, 3>;

                        var x = variables[instruction.arguments[0]];
                        var y = variables[instruction.arguments[1]];

                        typename component_type::input_type instance_input = {x, y};

                        component_type component_instance({0, 1, 2}, {}, {});

                        components::generate_circuit<BlueprintFieldType, ArithmetizationParams>(
                            component_instance, bp, assignmnt, instance_input, start_row);
                        typename component_type::result_type component_result =
                            components::generate_assignments<BlueprintFieldType, ArithmetizationParams>(
                                component_instance, assignmnt, instance_input, start_row);

                        variables[instruction.arguments[2]] = component_result.output;
                        break;
                    }
                    case BlueprintInstr::opcode_type::FMUL: {
                        using component_type = components::multiplication<ArithmetizationType, 3>;

                        var x = variables[instruction.arguments[0]];
                        var y = variables[instruction.arguments[1]];

                        typename component_type::input_type instance_input = {x, y};

                        component_type component_instance({0, 1, 2}, {}, {});

                        components::generate_circuit<BlueprintFieldType, ArithmetizationParams>(
                            component_instance, bp, assignmnt, instance_input, start_row);
                        typename component_type::result_type component_result =
                            components::generate_assignments<BlueprintFieldType, ArithmetizationParams>(
                                component_instance, assignmnt, instance_input, start_row);

                        variables[instruction.arguments[2]] = component_result.output;
                        break;
                    }
                    case BlueprintInstr::opcode_type::FDIV: {
                        using component_type = components::division<ArithmetizationType, 4>;

                        var x = variables[instruction.arguments[0]];
                        var y = variables[instruction.arguments[1]];

                        typename component_type::input_type instance_input = {x, y};

                        component_type component_instance({0, 1, 2, 3}, {}, {});

                        components::generate_circuit<BlueprintFieldType, ArithmetizationParams>(
                            component_instance, bp, assignmnt, instance_input, start_row);
                        typename component_type::result_type component_result =
                            components::generate_assignments<BlueprintFieldType, ArithmetizationParams>(
                                component_instance, assignmnt, instance_input, start_row);

                        variables[instruction.arguments[2]] = component_result.output;
                        break;
                    }
                    case BlueprintInstr::opcode_type::POSEIDON: {
                        using component_type = components::poseidon<ArithmetizationType, BlueprintFieldType, 15>;

                        std::array<var, component_type::state_size> input_state_var;
                        for (std::uint32_t i = 0; i < component_type::state_size; i++) {
                            input_state_var[i] = variables[instruction.arguments[i]];
                        }

                        typename component_type::input_type instance_input = {input_state_var};

                        component_type component_instance({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}, {}, {});

                        components::generate_circuit<BlueprintFieldType, ArithmetizationParams>(
                            component_instance, bp, assignmnt, instance_input, start_row);

                        typename component_type::result_type component_result =
                            components::generate_assignments<BlueprintFieldType, ArithmetizationParams>(
                                component_instance, assignmnt, instance_input, start_row);

                        for (std::uint32_t i = 0; i < component_type::state_size; i++) {
                            variables[instruction.arguments[component_type::state_size + i]] =
                                component_result.output_state[i];
                        }
                        break;
                    }
                    default:
                        assert(1 == 0 && "unsupported opcode type");
                }
            }

        public:
            template<typename PublicInputContainerType>
            bool evaluate(const BlueprintCode &code, const PublicInputContainerType &public_input) {

                std::map<std::string, var> variables;
                if (code.input_size != public_input.size()) {
                    std::cerr << "Public input must match the size of arguments" << std::endl;
                    return false;
                }

                for (std::size_t i = 0; i < public_input.size(); i++) {
                    assignmnt.public_input(0, i) = (public_input[i]);
                    variables[code.instructions[0].arguments[i]] = var(0, i, false, var::column_type::public_input);
                }

                for (std::int32_t instruction_index = 0; instruction_index < code.instructions.size(); instruction_index++) {
                    parse_instruction(variables, code.instructions[instruction_index]);
                }
                return true;
            }
        };

    }    // namespace blueprint
}    // namespace nil

#endif    // CRYPTO3_BLUEPRINT_COMPONENT_INSTRUCTION_PARSER_HPP
