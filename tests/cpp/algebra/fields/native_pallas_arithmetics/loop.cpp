#ifndef __ZKLLVM__
#include "../../../read_boost_json.hpp"
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] typename pallas::base_field_type::value_type
    arithmetics_loop(typename pallas::base_field_type::value_type a,
                        typename pallas::base_field_type::value_type b) {

    typename pallas::base_field_type::value_type c = a + b;

    for (int i = 0; i < 10; i++) {
        c = a + b;
        b = c * a;
        a = b - c;
    }

    #ifndef __ZKLLVM__
    std::cout << a.data <<std::endl;
    #endif

    return a;
}

#ifndef __ZKLLVM__

int main (int argc, char *argv[]){
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    std::string input_file_name(argv[1]);

    constexpr static const std::size_t input_size = 2;

    using BlueprintFieldType = typename pallas::base_field_type;

    std::vector<typename BlueprintFieldType::value_type> input_vec = read_fields<BlueprintFieldType>(input_file_name);
    if (input_vec.size() != input_size){
        std::cerr << "input file contains " << input_vec.size() << " field elements" << "\n";
        std::cerr << "addition circuit accepts " << input_size << " field elements" << "\n";
        assert(false && "input file does not match circuit signature");
    } else{
        arithmetics_loop(input_vec[0], input_vec[1]);
        return 0;
    }
}
#endif
