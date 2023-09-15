#ifndef __ZKLLVM__
#include "../../../read_boost_json.hpp"
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] typename pallas::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type constructor_test() {

#ifdef __ZKLLVM__
    typename pallas::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type B =
        pallas::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::one();
#else
    typename pallas::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type B =
        pallas::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type::one();
#endif

    #ifndef __ZKLLVM__
    std::cout << B.X.data <<std::endl;
    std::cout << B.Y.data <<std::endl;
    #endif

    return B;
}

#ifndef __ZKLLVM__

int main (int argc, char *argv[]){
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    constructor_test();
    return 0;
}

#endif
