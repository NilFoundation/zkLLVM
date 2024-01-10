#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

constexpr std::size_t size = 6;

typedef std::array<typename pallas::base_field_type::value_type, size> array_type;

[[circuit]] array_type swap_array(bool is_swap, array_type input) {

    array_type res;

    __builtin_assigner_fri_array_swap(res.data(), size, input.data(), is_swap);

    return res;
}
