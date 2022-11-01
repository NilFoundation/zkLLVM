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

using namespace nil;
using namespace nil::crypto3;

int main(int argc, char *argv[]) {
    using curve_type = algebra::curves::pallas;
    using BlueprintFieldType = typename curve_type::base_field_type;
    constexpr std::size_t WitnessColumns = 15;
    constexpr std::size_t PublicInputColumns = 5;
    constexpr std::size_t ConstantColumns = 5;
    constexpr std::size_t SelectorColumns = 20;

    using ArithmetizationParams =
        zk::snark::plonk_arithmetization_params<WitnessColumns, PublicInputColumns, ConstantColumns, SelectorColumns>;

    std::vector<nil::blueprint::BlueprintInstr> code = {
        {nil::blueprint::BlueprintInstr::POSEIDON, {"v0", "v1", "v2", "v3", "v4", "v5"}},
        {nil::blueprint::BlueprintInstr::FADD, {"v0", "v1", "v8"}},
        {nil::blueprint::BlueprintInstr::FMUL, {"v0", "v1", "v3"}},
        {nil::blueprint::BlueprintInstr::FMUL, {"v0", "v2", "v4"}},
        {nil::blueprint::BlueprintInstr::FADD, {"v1", "v3", "v5"}},
        {nil::blueprint::BlueprintInstr::FMUL, {"v4", "v5", "v6"}}};

    std::vector<typename BlueprintFieldType::value_type> public_input = {0, 1, 1};
    nil::blueprint::parser<BlueprintFieldType, ArithmetizationParams> parser_instance;

    parser_instance.evaluate(code, public_input);

    zk::snark::plonk_table_description<BlueprintFieldType, ArithmetizationParams> desc;
    desc.usable_rows_amount = parser_instance.assignment.rows_amount();
    desc.rows_amount = zk::snark::basic_padding(parser_instance.assignment);
    std::cout << "Usable rows: " << desc.usable_rows_amount << std::endl;
    std::cout << "Padded rows: " << desc.rows_amount << std::endl;

    nil::blueprint::profiling(parser_instance.assignment);

    return !nil::blueprint::is_satisfied(parser_instance.bp, parser_instance.assignment);
}