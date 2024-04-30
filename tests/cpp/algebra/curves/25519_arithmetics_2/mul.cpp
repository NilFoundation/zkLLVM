#ifndef __ZKLLVM__
#include "../../../read_boost_json.hpp"
#include <fstream>
#endif

#include <nil/crypto3/algebra/fields/curve25519/base_field.hpp>
#include <nil/crypto3/algebra/fields/curve25519/scalar_field.hpp>
#include <nil/crypto3/algebra/curves/ed25519.hpp>
#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

using curve_type = ed25519;
using curve_point_type =
    typename curve_type::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type;
using base_type = typename curve_type::base_field_type;
using scalar_type = typename curve_type::scalar_field_type;

[[circuit]] curve_point_type curve_mul(curve_point_type P, scalar_type::value_type s) {

    curve_point_type R = P * s;

#ifndef __ZKLLVM__
    std::cout << R.X.data << std::endl;
    std::cout << R.Y.data << std::endl;
#endif

    return R;
}

#ifndef __ZKLLVM__
int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    boost::json::value input_json = read_boost_json(std::string(argv[1]));

    curve_point_type P = read_curve<curve_point_type, base_type>(input_json, 0);
    scalar_type::value_type s = read_field<scalar_type>(input_json, 1);

    curve_mul(P, s);
    return 0;
}

#endif
