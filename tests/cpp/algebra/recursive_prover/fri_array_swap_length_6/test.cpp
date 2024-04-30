#ifndef __ZKLLVM__
#include "../../../read_boost_json.hpp"
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

constexpr std::size_t size = 6;

typedef std::array<typename pallas::base_field_type::value_type, size> array_type;

[[circuit]] array_type swap_array(bool swap, array_type input) {

    array_type res;

#ifdef __ZKLLVM__
    __builtin_assigner_fri_array_swap(res.data(), size, input.data(), swap);
    for (std::size_t i = 0; i < size; i++) {
        __builtin_assigner_print_native_pallas_field(res[i]);
    }
#else
    for (std::size_t i = 0; i < size / 2; i++) {
        if (swap) {
            std::cout << input[2 * i + 1].data << "\n";
            std::cout << input[2 * i].data << "\n";
        } else {
            std::cout << input[2 * i].data << "\n";
            std::cout << input[2 * i + 1].data << "\n";
        }
    }
    res = input;
#endif
    return res;
}

#ifndef __ZKLLVM__

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    boost::json::value input_json = read_boost_json(std::string(argv[1]));

    using BlueprintFieldType = typename pallas::base_field_type;
    bool swap = read_uint<uint32_t>(input_json, 0);
    std::array<typename pallas::base_field_type::value_type, size> input =
        read_array_field<BlueprintFieldType, size>(input_json, 1);

    swap_array(swap, input);
    return 0;
}
#endif
