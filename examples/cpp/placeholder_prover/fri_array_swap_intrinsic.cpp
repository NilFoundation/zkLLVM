#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

template<typename input_type>
input_type swap_array(input_type input, bool swap) {
    input_type res;
    for (std::size_t i = 0; i < res.size(); i+=2) {
        if (swap) {
            res[i] = input[i+1];
            res[i+1] = input[i];
        }
        else {
            res[i] = input[i];
            res[i+1] = input[i+1];
        }
    }
    return res;
}

constexpr std::size_t size = 6;

typedef std::array<typename pallas::base_field_type::value_type, size> array_type;

[[circuit]] array_type swap_array(bool is_swap, array_type input) {

    array_type res;

    __builtin_assigner_fri_array_swap(res.data(), size, input.data(), is_swap);

    return res;
}
