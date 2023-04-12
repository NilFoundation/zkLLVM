#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

constexpr std::size_t VertexAmount = 5;
[[circuit]] std::array<typename pallas::base_field_type::value_type, 2>
    balance(std::array<typename pallas::base_field_type::value_type, 2 * VertexAmount> vertexes,
            std::array<typename pallas::base_field_type::value_type, VertexAmount>
                weights) {

    std::array<typename pallas::base_field_type::value_type, 2> balance_point = {0,0};
    typename pallas::base_field_type::value_type balance_weight = 0;

    for (std::size_t VertexIndex = 0; VertexIndex < VertexAmount; VertexIndex++) {
        balance_point[0] += vertexes[VertexIndex + VertexIndex] * weights[VertexIndex];
        balance_point[1] += vertexes[VertexIndex + VertexIndex + 1] * weights[VertexIndex];
        balance_weight += weights[VertexIndex];
    }

    balance_point[0] /= balance_weight;
    balance_point[1] /= balance_weight;

    return balance_point;
}
