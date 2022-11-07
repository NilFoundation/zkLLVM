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

#include <nil/blueprint/parser.hpp>
#include <nil/blueprint/utils/table_profiling.hpp>
#include <nil/blueprint/utils/satisfiability_check.hpp>
#include <nil/blueprint/ir_translator.hpp>

using namespace nil;
using namespace nil::crypto3;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " ir_file" << std::endl;
        return 1;
    }

    using curve_type = algebra::curves::pallas;
    using BlueprintFieldType = typename curve_type::base_field_type;
    constexpr std::size_t WitnessColumns = 15;
    constexpr std::size_t PublicInputColumns = 5;
    constexpr std::size_t ConstantColumns = 5;
    constexpr std::size_t SelectorColumns = 20;

    using ArithmetizationParams =
        zk::snark::plonk_arithmetization_params<WitnessColumns, PublicInputColumns, ConstantColumns, SelectorColumns>;

    std::string translation_error;
    auto code = blueprint::ir_translator::translate_ir_file(argv[1], translation_error);
    if (code == nullptr) {
        std::cerr << translation_error << std::endl;
        return 1;
    }

    code->dump();

    std::vector<typename BlueprintFieldType::value_type> public_input = {1, 11};
    nil::blueprint::parser<BlueprintFieldType, ArithmetizationParams> parser_instance;

    if (!parser_instance.evaluate(*code, public_input)) {
        return 1;
    }

    zk::snark::plonk_table_description<BlueprintFieldType, ArithmetizationParams> desc;
    desc.usable_rows_amount = parser_instance.assignmnt.rows_amount();
    desc.rows_amount = zk::snark::basic_padding(parser_instance.assignmnt);
    std::cout << "Usable rows: " << desc.usable_rows_amount << std::endl;
    std::cout << "Padded rows: " << desc.rows_amount << std::endl;

    nil::blueprint::profiling(parser_instance.assignmnt);

    return !nil::blueprint::is_satisfied(parser_instance.bp, parser_instance.assignmnt);
}
