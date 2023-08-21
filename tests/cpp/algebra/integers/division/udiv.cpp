#ifndef __ZKLLVM__
#include "../../../read_boost_json.hpp"
#include <cstdint>
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

[[circuit]] uint32_t division(uint32_t a, uint32_t b) {

    uint32_t c = a / b;

    #ifndef __ZKLLVM__
    std::cout << c <<std::endl;
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

    std::vector<uint32_t> input_vec = read_uint32_t(input_file_name);
    if (input_vec.size() != input_size){
        std::cerr << "input file contains " << input_vec.size() << " integers" << "\n";
        std::cerr << "circuit accepts " << input_size << " integers" << "\n";
        assert(false && "input file does not match circuit signature");
    } else{
        division(input_vec[0], input_vec[1]);
        return 0;
    }
}
#endif
