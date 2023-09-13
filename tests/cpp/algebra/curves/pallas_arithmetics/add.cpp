#ifndef __ZKLLVM__
#include "../../../read_boost_json.hpp"
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] typename pallas::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type curve_addition(
        typename pallas::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type a,
        typename pallas::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type b) {

    typename pallas::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type c = a + b;

    #ifndef __ZKLLVM__
    std::cout << c.X.data <<std::endl;
    std::cout << c.Y.data <<std::endl;
    #endif

    return c;
}

#ifndef __ZKLLVM__

int main (int argc, char *argv[]){
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    using curve_type = pallas;
    using curve_point_type = typename curve_type::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type;
    using field_type = typename curve_type::base_field_type;

    std::string input_file_name(argv[1]);

    constexpr static const std::size_t input_size = 2;

    std::vector<curve_point_type> input_vec = read_curves<curve_point_type, field_type>(input_file_name);
    if (input_vec.size() != input_size){
        std::cerr << "input file contains " << input_vec.size() << " field elements" << "\n";
        std::cerr << "addition circuit accepts " << input_size << " field elements" << "\n";
        assert(false && "input file does not match circuit signature");
    } else{
        curve_addition(input_vec[0], input_vec[1]);
        return 0;
    }
}
#endif
