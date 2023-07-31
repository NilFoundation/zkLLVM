// #ifndef __ZKLLVM__
//   #include "../../read_boost_json.hpp"
//   #include <fstream>
// #endif

// #include <nil/crypto3/algebra/curves/pallas.hpp>

// using namespace nil::crypto3::algebra::curves;

// [[circuit]] typename pallas::base_field_type::value_type
//     pallas_field_add(typename pallas::base_field_type::value_type a,
//                         typename pallas::base_field_type::value_type b) {

//     typename pallas::base_field_type::value_type c = a + b;

//     #ifndef __ZKLLVM__
//     std::cout << c.data <<std::endl;
//     #endif

//     return c;
// }

// #ifndef __ZKLLVM__


// int main (){
//     std::string input_file_name = "native_pallas_field_add0.inp";

//     constexpr static const input_size = 2;

//     std::vector<typename pallas::base_field_type::value_type> input_vec = read_boost_json(input_file_name);
//     if (input_vec.size() != input_len){
//         std::cerr << "input file contains " << input_vec.size() << " field elements" << "\n";
//         std::cerr << "addition circuit accepts " << input_len << " field elements" << "\n";
//         UNREACHABLE("input file does not match circuit signature");
//     } else{
//         pallas_field_add(input_vec[0], input_vec[1]);
//         return 0;
//     }
// }
// #endif


#include "../../read_boost_json.hpp"
#include <fstream>

#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] typename pallas::base_field_type::value_type
    pallas_field_add(typename pallas::base_field_type::value_type a,
                        typename pallas::base_field_type::value_type b) {

    typename pallas::base_field_type::value_type c = a + b;

    std::cout << c.data <<std::endl;

    return c;
}

int main (){
    std::string input_file_name = "native_pallas_field_add0.inp";

    constexpr static const std::size_t input_size = 2;

    using BlueprintFieldType = typename pallas::base_field_type;

    std::vector<typename BlueprintFieldType::value_type> input_vec = read_fields<BlueprintFieldType>(input_file_name);
    if (input_vec.size() != input_size){
        std::cerr << "input file contains " << input_vec.size() << " field elements" << "\n";
        std::cerr << "addition circuit accepts " << input_size << " field elements" << "\n";
        assert(false && "input file does not match circuit signature");
    } else{
        pallas_field_add(input_vec[0], input_vec[1]);
        return 0;
    }
}
