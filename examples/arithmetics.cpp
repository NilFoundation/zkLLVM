namespace nil{
    namespace crypto3{
        namespace algebra {
            struct bls12381{
                typedef field_bls12381b value_type;
            };
        }
    }
}

using namespace nil::crypto3;

[[circuit]] typename algebra::bls12381::value_type field_arithmetic_example(
	typename algebra::bls12381::value_type a,
	typename algebra::bls12381::value_type b) {

    typename algebra::bls12381::value_type c = (a + b)*a + b*(a+b)*(a+b);
    return c*c*c/(b - a);
}
