#ifndef __ZKLLVM__
#include "../../../read_boost_json.hpp"
#include <fstream>
#endif

#include <nil/crypto3/algebra/fields/curve25519/base_field.hpp>
#include <nil/crypto3/algebra/fields/curve25519/scalar_field.hpp>
#include <nil/crypto3/algebra/curves/ed25519.hpp>
#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] bool curve_comparison(
    typename ed25519::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type a,
    typename ed25519::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type b) {

    bool c = a == b;

#ifndef __ZKLLVM__
    std::cout << c << std::endl;
#endif

    return c;
}

#ifndef __ZKLLVM__
int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    using curve_type = ed25519;
    using curve_point_type =
        typename curve_type::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type;
    using field_type = typename curve_type::base_field_type;

    boost::json::value input_json = read_boost_json(std::string(argv[1]));

    curve_point_type a = read_curve<curve_point_type, field_type>(input_json, 0);
    curve_point_type b = read_curve<curve_point_type, field_type>(input_json, 1);

    curve_comparison(a, b);
    return 0;
}

#endif
