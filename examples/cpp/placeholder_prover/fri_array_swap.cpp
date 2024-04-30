#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

template<typename input_type, size_t size>
input_type swap_array(input_type input, bool swap) {
    input_type res;
    if (swap) {
        for (std::size_t i = 0; i < size; i += 2) {
            res[i] = input[i + 1];
            res[i + 1] = input[i];
        }
    } else {
        for (std::size_t i = 0; i < size; i += 2) {
            res[i] = input[i];
            res[i + 1] = input[i + 1];
        }
    }
    return res;
}

[[circuit]] typename pallas::base_field_type::value_type
    field_arithmetic_example(bool swap, std::array<typename pallas::base_field_type::value_type, 6> input) {

    using arr6_type = std::array<typename pallas::base_field_type::value_type, 6>;
    using arr4_type = std::array<typename pallas::base_field_type::value_type, 4>;

    arr6_type arr1 = swap_array<arr6_type, 6>(input, swap);
    arr4_type arr2;

    for (std::size_t i = 0; i < 4; i++) {
        arr2[i] = arr1[i];
    }

    arr4_type arr3 = swap_array<arr4_type, 4>(arr2, !swap);

    return arr1[1] + arr2[2] + arr3[3];
}
