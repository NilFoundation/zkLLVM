#ifndef __ZKLLVM__
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/ed25519.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] typename ed25519::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type
    constructor_test() {

#ifdef __ZKLLVM__
    typename ed25519::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type B =
        ed25519::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::zero();
#else
    typename ed25519::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type B =
        ed25519::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type::zero();
#endif

#ifndef __ZKLLVM__
    std::cout << B.X.data << std::endl;
    std::cout << B.Y.data << std::endl;
#endif

    return B;
}

#ifndef __ZKLLVM__

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    constructor_test();
    return 0;
}

#endif
