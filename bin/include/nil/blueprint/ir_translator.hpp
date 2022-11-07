//---------------------------------------------------------------------------//
// Copyright (c) 2022 Mikhail Aksenov <maksenov@nil.foundation>
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

#ifndef CRYPTO3_BLUEPRINT_IR_TRANSLATOR_HPP
#define CRYPTO3_BLUEPRINT_IR_TRANSLATOR_HPP

#include <nil/blueprint/parser.hpp>

#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/InstrTypes.h>

namespace nil::blueprint {

    class ir_translator {
    public:
        static std::unique_ptr<BlueprintCode> translate_ir_file(const char *filename, /*out*/ std::string &error) {
            ir_translator translator;
            return translator.translate(filename, error);
        }

        std::unique_ptr<BlueprintCode> translate(const char *filename, /*out*/ std::string &error) {
            llvm::LLVMContext context;
            llvm::SMDiagnostic diagnostic;
            std::unique_ptr<llvm::Module> module = llvm::parseIRFile(filename, diagnostic, context);
            if (module == nullptr) {
                error = "Unable to parse IR file: " + diagnostic.getMessage().str();
                return nullptr;
            }
            if (module->size() != 1) {
                error = "IR module must contain only one function";
                return nullptr;
            }
            llvm::Function &ir_function = *module->begin();

            // Public input for blueprint must match the size of arguments
            result->input_size = ir_function.arg_size();

            // Add registers for function arguments
            for (int i = 0; i < ir_function.arg_size(); ++i) {
                get_or_add_reg(ir_function.getArg(i));
            }
            if (ir_function.size() != 1) {
                // TODO(maksenov): support several basic blocks
                error = "Functions with several basic blocks are not supported yet";
                return nullptr;
            }
            BlueprintInstr cur_inst;
            for (auto &inst : ir_function.front()) {
                // std::cout << inst.getOpcodeName() << std::endl;
                cur_inst.arguments.clear();
                switch (inst.getOpcode()) {
                    case llvm::Instruction::Add:
                    case llvm::Instruction::Sub:
                    case llvm::Instruction::Mul:
                    case llvm::Instruction::SDiv:
                    case llvm::Instruction::UDiv:
                        cur_inst.opcode = translate_opcode(inst.getOpcode());
                        // Add instruction operands
                        cur_inst.arguments.push_back(get_or_add_reg(inst.getOperand(0)));
                        cur_inst.arguments.push_back(get_or_add_reg(inst.getOperand(1)));
                        // Add result
                        cur_inst.arguments.push_back(get_or_add_reg(&inst));

                        result->instructions.push_back(cur_inst);
                        break;
                    case llvm::Instruction::Ret:
                        break;
                    default:
                        error = "Unhandled instruction: " + inst.getName().str();
                        return nullptr;
                }
            }
            return std::move(result);
        }

    private:
        std::string get_or_add_reg(llvm::Value *value) {
            auto it = register_map.find(value);
            if (it != register_map.end()) {
                return it->second;
            }
            std::string new_reg = "v" + std::to_string(register_counter++);
            register_map[value] = new_reg;
            return new_reg;
        }

        BlueprintInstr::opcode_type translate_opcode(unsigned ir_opcode) {
            switch (ir_opcode) {
                case llvm::Instruction::Add:
                    return BlueprintInstr::FADD;
                case llvm::Instruction::Sub:
                    return BlueprintInstr::FSUB;
                case llvm::Instruction::Mul:
                    return BlueprintInstr::FMUL;
                case llvm::Instruction::SDiv:
                case llvm::Instruction::UDiv:
                    return BlueprintInstr::FDIV;
                default:
                    llvm::llvm_unreachable_internal();
            }
        }

    private:
        std::unique_ptr<BlueprintCode> result = std::make_unique<BlueprintCode>();
        std::unordered_map<llvm::Value *, std::string> register_map;
        int register_counter = 0;
    };


}  // namespace nil::blueprint

#endif  // CRYPTO3_BLUEPRINT_IR_TRANSLATOR_HPP
