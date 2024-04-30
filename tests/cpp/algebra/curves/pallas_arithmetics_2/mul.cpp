#ifndef __ZKLLVM__
#include "../../../read_boost_json.hpp"
#include <fstream>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] typename pallas::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type
    curve_mul(typename pallas::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type point,
              typename pallas::scalar_field_type::value_type scalar) {

    typename pallas::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type res =
        point * scalar;

#ifndef __ZKLLVM__
    std::cout << res.X.data << std::endl;
    std::cout << res.Y.data << std::endl;
#endif

    return res;
}

#ifndef __ZKLLVM__

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    using curve_type = pallas;
    using curve_point_type =
        typename curve_type::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type;
    using base_type = typename curve_type::base_field_type;
    using scalar_type = typename curve_type::scalar_field_type;

    boost::json::value input_json = read_boost_json(std::string(argv[1]));

    curve_point_type point = read_curve<curve_point_type, base_type>(input_json, 0);
    scalar_type::value_type scalar = read_field<scalar_type>(input_json, 1);

    curve_mul(point, scalar);
    return 0;
}

#endif
