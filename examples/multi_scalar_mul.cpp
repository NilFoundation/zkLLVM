#include<array>

constexpr static const std::size_t msm_size = 5;

[[circuit]]__zkllvm_curve_pallas msm(
    std::array<__zkllvm_curve_pallas, msm_size> points,
    std::array<__zkllvm_field_pallas_scalar, msm_size> scalars) {
        
        __zkllvm_curve_pallas first = points[0] * scalars[0];
        __zkllvm_curve_pallas current_point;
        __zkllvm_curve_pallas sum = first;

        for (std::size_t i = 1; i < msm_size; i++) {
            current_point = points[i] * scalars[i];
            sum = sum + current_point;
        }

        return sum;
}