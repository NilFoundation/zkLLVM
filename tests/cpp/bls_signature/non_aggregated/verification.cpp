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

bool check_equality(
    typename algebra::curves::bls12<381>::gt_type::value_type pairing1,
    typename algebra::curves::bls12<381>::gt_type::value_type pairing2
) {

#ifdef __ZKLLVM__
    bool are_equal = (pairing1[0] == pairing2[0]);
    for(std::size_t i = 1; i < 12; i++) {
        are_equal = are_equal && (pairing1[i] == pairing2[i]);
    }
#else
    bool are_equal = (pairing1 == pairing2);

    std::cout << are_equal <<std::endl;
#endif
    return are_equal;
}

[[circuit]] bool verify_signature(
    typename algebra::fields::bls12_base_field<381>::value_type hashed_msg,
    typename algebra::curves::bls12<381>::template g2_type<>::value_type pubkey,
    typename algebra::curves::bls12<381>::template g1_type<>::value_type sig) {

#ifdef __ZKLLVM__
    typename algebra::curves::bls12<381>::template g1_type<>::value_type msg_point = __builtin_assigner_hash_to_curve(hashed_msg);

    __builtin_assigner_is_in_g1_check(sig);
    __builtin_assigner_is_in_g2_check(pubkey);

    typename algebra::curves::bls12<381>::template g2_type<>::value_type g2_group_generator = algebra::curves::bls12<381>::template g2_type<>::one();
#else
    typename algebra::curves::bls12<381>::template g1_type<>::value_type msg_point;
    typename algebra::curves::bls12<381>::template g2_type<>::value_type g2_group_generator = algebra::curves::bls12<381>::template g2_type<>::value_type::one();
#endif

    typename algebra::curves::bls12<381>::gt_type::value_type pairing1 = algebra::pair<algebra::curves::bls12<381>>(sig, g2_group_generator);
    typename algebra::curves::bls12<381>::gt_type::value_type pairing2 = algebra::pair<algebra::curves::bls12<381>>(msg_point, pubkey);

    return check_equality(pairing1, pairing2);
}


#ifndef __ZKLLVM__
int main (int argc, char *argv[]){
    if (argc != 2) {
        std::cerr << "one command line argument must be provided\n";
        std::abort();
    }

    using curve_type = bls12<381>;
    using curve_point_type = typename curve_type::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type;
    // using field_type = typename curve_type::base_field_type;

    // boost::json::value input_json = read_boost_json(std::string(argv[1]));

    // curve_point_type a = read_curve<curve_point_type, field_type>(input_json, 0);
    // curve_point_type b = read_curve<curve_point_type, field_type>(input_json, 1);

    typename algebra::fields::bls12_base_field<381>::value_type hashed_msg;
    typename algebra::curves::bls12<381>::template g2_type<>::value_type pubkey;
    typename algebra::curves::bls12<381>::template g1_type<>::value_type sig;

    verify_signature(hashed_msg, pubkey, sig);
    return 0;
}

#endif
