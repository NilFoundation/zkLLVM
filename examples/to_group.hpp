#include<array>
#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

typename pallas::base_field_type::value_type sqrt(typename pallas::base_field_type::value_type y) {
    // TODO implement function
}

typename pallas::base_field_type::value_type to_group(typename pallas::base_field_type::value_type t) {
    typename pallas::base_field_type::value_type zero = 0;
    typename pallas::base_field_type::value_type one = 1;
    typename pallas::base_field_type::value_type minus_one = -1;

    //pallas:
    typename pallas::base_field_type::value_type u = 0x0000000000000000000000000000000000000000000000000000000000000001_cppui255;
    typename pallas::base_field_type::value_type fu = 0x0000000000000000000000000000000000000000000000000000000000000006_cppui255;
    typename pallas::base_field_type::value_type inv_three_u_squared = 0x2AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC18465FD5B88A612661E209E00000001_cppui255;
    typename pallas::base_field_type::value_type sqrt_neg_three_u_squared = 0x25999506959B74E25955ABB8AF5563603A3F17A46F5A62923B5ABD7BFBFC9573_cppui255;
    typename pallas::base_field_type::value_type sqrt_neg_three_u_squared_minus_u_over_2 = 0x12CCCA834ACDBA712CAAD5DC57AAB1B01D1F8BD237AD31491DAD5EBDFDFE4AB9_cppui255;
    typename pallas::base_field_type::value_type b = 0x0000000000000000000000000000000000000000000000000000000000000005_cppui255;

    // vesta
    // __zkllvm_field_pallas_base u = 0x0000000000000000000000000000000000000000000000000000000000000001_cppui255;
    // __zkllvm_field_pallas_base fu = 0x0000000000000000000000000000000000000000000000000000000000000006_cppui255;
    // __zkllvm_field_pallas_base inv_three_u_squared = 0x2AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC18465FD5BB87093B2D9F21600000001_cppui255;
    // __zkllvm_field_pallas_base sqrt_neg_three_u_squared = 0x0D0334B0507CA51CA23B69B039EE1EB41FDA8CFA8F80675E5553A5C0A1541C9F_cppui255;
    // __zkllvm_field_pallas_base sqrt_neg_three_u_squared_minus_u_over_2 = 0x06819A58283E528E511DB4D81CF70F5A0FED467D47C033AF2AA9D2E050AA0E4F_cppui255;
    // __zkllvm_field_pallas_base b = 0x0000000000000000000000000000000000000000000000000000000000000005_cppui255;
    


    typename pallas::base_field_type::value_type alpha = 1 / (((t * t) + fu) * (t * t));
    std::array<typename pallas::base_field_type::value_type, 3> xs;
    xs[0] = sqrt_neg_three_u_squared_minus_u_over_2 - t*t*t*t * alpha * sqrt_neg_three_u_squared; // sqrt(-3u^2-u/2) - t2^2 * alpha * sqrt(-3u^2)
    xs[1] = 0 - u - xs[0];
    xs[2] = u - (t * t + fu) * (t * t + fu) * (t * t + fu) * alpha * inv_three_u_squared;
                        
    std::array<typename pallas::base_field_type::value_type, 3> ys;
    for (std::size_t i = 0; i < xs.size(); ++i) {
        ys[i] = sqrt(xs[i]*xs[i]*xs[i] + b); // x^3 + A*x + B = y^2, A=0 for pasta curves
    }


    typename pallas::base_field_type::value_type res_x = 0;
    typename pallas::base_field_type::value_type res_y = 0;


    for (std::size_t i = 0; i < xs.size(); ++i) {
        if (ys[i] != -1) {
            res_x = xs[i]; 
            res_y = ys[i]; 
            break;
        }
    }

    // TODO: implement curve_enement from two field_elements
    // return {res_x, res_y};
}