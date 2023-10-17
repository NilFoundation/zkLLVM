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


[[circuit]] typename pallas::base_field_type::value_type
    field_arithmetic_example(typename pallas::base_field_type::value_type swap_field,
        std::array<typename pallas::base_field_type::value_type, 6> input) {

    bool swap = typename pallas::base_field_type::value_type(0) != swap_field;

    using arr6_type = std::array<typename pallas::base_field_type::value_type, 6>;
    using arr4_type = std::array<typename pallas::base_field_type::value_type, 4>;

    arr6_type arr1 = swap_array<arr6_type> (input, swap);
    arr4_type arr2;

    for (std::size_t i = 0; i < arr2.size(); i++) {
        arr2[i] = arr1[i];
    }

    arr4_type arr3 = swap_array<arr4_type> (arr2, !swap);

    return arr1[1] + arr2[2] + arr3[3];
}
