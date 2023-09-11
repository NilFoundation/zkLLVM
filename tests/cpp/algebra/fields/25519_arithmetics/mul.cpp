#ifndef __ZKLLVM__
#include "../../../read_boost_json.hpp"
#include <fstream>
#endif

#include <nil/crypto3/algebra/fields/curve25519/base_field.hpp>
#include <nil/crypto3/algebra/curves/ed25519.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] typename ed25519::base_field_type::value_type field_addition(
        typename ed25519::base_field_type::value_type a,
        typename ed25519::base_field_type::value_type b) {

    typename ed25519::base_field_type::value_type c = a * b;

    #ifndef __ZKLLVM__
    std::cout << c.data <<std::endl;
    #endif

    return c;
}

#ifndef __ZKLLVM__

int main (int argc, char *argv[]){
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    std::string input_file_name(argv[1]);

    constexpr static const std::size_t input_size = 2;

    std::vector<typename ed25519::base_field_type::value_type> input_vec = read_fields<typename ed25519::base_field_type>(input_file_name);
    if (input_vec.size() != input_size){
        std::cerr << "input file contains " << input_vec.size() << " field elements" << "\n";
        std::cerr << "addition circuit accepts " << input_size << " field elements" << "\n";
        assert(false && "input file does not match circuit signature");
    } else{
        field_addition(input_vec[0], input_vec[1]);
        return 0;
    }
}
#endif
