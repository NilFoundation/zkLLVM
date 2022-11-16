
namespace nil{
    namespace crypto3{
        namespace algebra {
            struct bls12381{
                typedef field_bls12381b value_type;
            };
        }

        namespace hashes {
            typedef __attribute__((ext_vector_type(8))) typename algebra::bls12381::value_type sha512_state_type;
            sha512_state_type sha512(algebra::bls12381::value_type s0,
                                     algebra::bls12381::value_type s1,
                                     algebra::bls12381::value_type s2,
                                     algebra::bls12381::value_type s3,
                                     algebra::bls12381::value_type s4,
                                     algebra::bls12381::value_type s5,
                                     algebra::bls12381::value_type s6,
                                     algebra::bls12381::value_type s7,
                                     algebra::bls12381::value_type i0,
                                     algebra::bls12381::value_type i1,
                                     algebra::bls12381::value_type i2,
                                     algebra::bls12381::value_type i3,
                                     algebra::bls12381::value_type i4,
                                     algebra::bls12381::value_type i5,
                                     algebra::bls12381::value_type i6,
                                     algebra::bls12381::value_type i7,
                                     algebra::bls12381::value_type i8,
                                     algebra::bls12381::value_type i9,
                                     algebra::bls12381::value_type i10,
                                     algebra::bls12381::value_type i11,
                                     algebra::bls12381::value_type i12,
                                     algebra::bls12381::value_type i13,
                                     algebra::bls12381::value_type i14,
                                     algebra::bls12381::value_type i15);
        }
    }
}

using namespace nil::crypto3;

typename hashes::sha512_state_type sha512_example(algebra::bls12381::value_type s0,
                                     algebra::bls12381::value_type s1,
                                     algebra::bls12381::value_type s2,
                                     algebra::bls12381::value_type s3,
                                     algebra::bls12381::value_type s4,
                                     algebra::bls12381::value_type s5,
                                     algebra::bls12381::value_type s6,
                                     algebra::bls12381::value_type s7,
                                     algebra::bls12381::value_type i0,
                                     algebra::bls12381::value_type i1,
                                     algebra::bls12381::value_type i2,
                                     algebra::bls12381::value_type i3,
                                     algebra::bls12381::value_type i4,
                                     algebra::bls12381::value_type i5,
                                     algebra::bls12381::value_type i6,
                                     algebra::bls12381::value_type i7,
                                     algebra::bls12381::value_type i8,
                                     algebra::bls12381::value_type i9,
                                     algebra::bls12381::value_type i10,
                                     algebra::bls12381::value_type i11,
                                     algebra::bls12381::value_type i12,
                                     algebra::bls12381::value_type i13,
                                     algebra::bls12381::value_type i14,
                                     algebra::bls12381::value_type i15) {

    typename hashes::sha512_state_type hash_result = hashes::sha512(
        s0, s1, s2, s3, s4, s5, s6, s7, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15);


    return hash_result;
}
