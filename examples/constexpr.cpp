#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

consteval typename pallas::base_field_type::value_type
    polynomial_eval(typename pallas::base_field_type::value_type factor_coeff,
                    typename pallas::base_field_type::value_type root_value, int power = 50) {
    typename pallas::base_field_type::value_type root_power = 1;
    typename pallas::base_field_type::value_type factor_power = 1;
    typename pallas::base_field_type::value_type result = 0;
    for (int i = 0; i <= power; i++){
        result += root_power*factor_power;
        factor_power *= factor_coeff;
        root_power *= root_value;
    }
    return result;
}

[[circuit]] typename pallas::base_field_type::value_type
    polynomial_sbox(typename pallas::base_field_type::value_type scale,
                    typename pallas::base_field_type::value_type salt) {
    constexpr typename pallas::base_field_type::value_type seed = 5;
    constexpr typename pallas::base_field_type::value_type root = 15;
    constexpr int sbox_power = 7;
    return (((polynomial_eval(seed, root, sbox_power) + salt) * scale + polynomial_eval(seed, root, sbox_power)) *
                scale +
            polynomial_eval(seed, root, sbox_power)) *
           scale;
}
