#ifndef __ZKLLVM__
#include "../../read_boost_json.hpp"
#include <fstream>
#include <nil/crypto3/algebra/pairing/bls12.hpp>
#endif

#include <nil/crypto3/algebra/fields/bls12/base_field.hpp>
#include <nil/crypto3/algebra/curves/bls12.hpp>
#include <nil/crypto3/algebra/algorithms/pair.hpp>

using namespace nil::crypto3;
using namespace nil::crypto3::algebra::curves;

[[circuit]] typename algebra::curves::bls12<381>::gt_type::value_type
    pairing_test(typename algebra::curves::bls12<381>::template g1_type<>::value_type g1,
                 typename algebra::curves::bls12<381>::template g2_type<>::value_type g2) {

    typename algebra::curves::bls12<381>::gt_type::value_type res = algebra::pair<algebra::curves::bls12<381>>(g1, g2);

#ifndef __ZKLLVM__
    std::cout << res.data[0].data[0].data[0].data << "\n";
    std::cout << res.data[0].data[0].data[1].data << "\n";
    std::cout << res.data[0].data[1].data[0].data << "\n";
    std::cout << res.data[0].data[1].data[1].data << "\n";
    std::cout << res.data[0].data[2].data[0].data << "\n";
    std::cout << res.data[0].data[2].data[1].data << "\n";
    std::cout << res.data[1].data[0].data[0].data << "\n";
    std::cout << res.data[1].data[0].data[1].data << "\n";
    std::cout << res.data[1].data[1].data[0].data << "\n";
    std::cout << res.data[1].data[1].data[1].data << "\n";
    std::cout << res.data[1].data[2].data[0].data << "\n";
    std::cout << res.data[1].data[2].data[1].data << "\n";
#endif
    return res;
}

#ifndef __ZKLLVM__
int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    boost::json::value input_json = read_boost_json(std::string(argv[1]));

    using blueprint_field_type = typename algebra::curves::bls12<381>::base_field_type;
    using g2_group_type = typename bls12<381>::g2_type<>;
    using curve_point_type = algebra::curves::bls12<381>::template g1_type<>::value_type;

    curve_point_type g1 = read_curve<curve_point_type, blueprint_field_type>(input_json, 0);
    g1.Z = bls12<381>::template g1_type<>::field_type::value_type::one();
    g2_group_type::value_type g2 = read_g2_curve<blueprint_field_type, g2_group_type>(input_json, 1);

    pairing_test(g1, g2);
    return 0;
}
#endif
