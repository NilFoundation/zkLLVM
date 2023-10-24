#ifndef __ZKLLVM__
#include "../../../read_boost_json.hpp"
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

constexpr bool is_msb = true;
constexpr std::size_t bits_amount = 64;

void print_field(typename pallas::base_field_type::value_type result_i) {
#ifdef __ZKLLVM__
    __builtin_assigner_print_native_pallas_field(result_i);
#else
    std::cout << result_i.data << "\n";
#endif
}

[[circuit]] std::array<typename pallas::base_field_type::value_type, bits_amount>
        decompose(uint64_t input) {

    std::array<typename pallas::base_field_type::value_type, bits_amount> result;

#ifdef __ZKLLVM__
    __builtin_assigner_bit_decomposition(result.data(), bits_amount, input, is_msb);
#else
    std::size_t shift = 0;
    for (std::size_t i = 0; i < bits_amount; i++) {
        shift = is_msb ? (bits_amount - 1 - i) : i;
        result[i] = (input >> shift) & 1;
    }
#endif

    for (std::size_t i = 0; i < bits_amount; i++) {
        print_field(result[i]);
    }

    return result;
}

#ifndef __ZKLLVM__

int main (int argc, char *argv[]){
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    boost::json::value input_json = read_boost_json(std::string(argv[1]));

    using BlueprintFieldType = typename pallas::base_field_type;
    uint64_t input = read_uint<uint64_t>(input_json, 0);

    decompose(input);
    return 0;
}
#endif

