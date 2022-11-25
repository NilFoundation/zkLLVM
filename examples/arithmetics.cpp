namespace nil{
    namespace crypto3{
        namespace algebra {
            namespace curves {
                class pallas {
                public:
                    typedef field_pallasb base_field_type;
                };
            }    // namespace curves
            struct bls12381 {
                typedef field_bls12381b value_type;
            };
        }
    }
}

using namespace nil::crypto3::algebra::curves;
using namespace nil::crypto3::algebra;
typename pallas::base_field_type square(typename pallas::base_field_type a)
{
    return a * a;
}

[[circuit]] typename pallas::base_field_type field_arithmetic_example(
	typename pallas::base_field_type a,
	typename pallas::base_field_type b) {

    typename pallas::base_field_type c = (a + b)*a + b*(a+b)*(a+b);
    return c*c*c/(b - a) + square(a);
}
