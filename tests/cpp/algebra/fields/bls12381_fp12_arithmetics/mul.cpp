#ifndef __ZKLLVM__
#include "../../../read_boost_json.hpp"
#include <fstream>
#include <nil/crypto3/algebra/pairing/bls12.hpp>
#endif

#include <nil/crypto3/algebra/fields/bls12/base_field.hpp>
#include <nil/crypto3/algebra/curves/bls12.hpp>
#include <nil/crypto3/algebra/algorithms/pair.hpp>

using namespace nil::crypto3;
using namespace nil::crypto3::algebra::curves;



[[circuit]] typename algebra::curves::bls12<381>::gt_type::value_type fp12_mul(
    typename algebra::curves::bls12<381>::gt_type::value_type x,
    typename algebra::curves::bls12<381>::gt_type::value_type y
) {

#ifdef __ZKLLVM__
    typename algebra::curves::bls12<381>::gt_type::value_type res = __builtin_assigner_gt_multiplication(x, y);
#else
    typename algebra::curves::bls12<381>::gt_type::value_type res = x * y;
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
int main (int argc, char *argv[]){
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    boost::json::value input_json = read_boost_json(std::string(argv[1]));

    using blueprint_field_type = typename algebra::curves::bls12<381>::base_field_type;
    using fp12_element = typename algebra::fields::fp12_2over3over2<blueprint_field_type>::value_type;

    fp12_element x = read_fp12<blueprint_field_type, fp12_element>(input_json, 0);
    fp12_element y = read_fp12<blueprint_field_type, fp12_element>(input_json, 1);


    fp12_mul(x, y);
    return 0;
}

#endif
