namespace nil{
    namespace crypto3{
        namespace algebra {
            struct bls12381{
                typedef field_bls12381b value_type;
            };
        }

        namespace hashes {
            typedef __attribute__((ext_vector_type(3))) typename algebra::bls12381::value_type poseidon_block_type;

            poseidon_block_type poseidon(algebra::bls12381::value_type a,
                                         algebra::bls12381::value_type b,
                                         algebra::bls12381::value_type c);
        }
    }
}

using namespace nil::crypto3;

typename hashes::poseidon_block_type poseidon_example1(
    typename algebra::bls12381::value_type a,
    typename algebra::bls12381::value_type b,
    typename algebra::bls12381::value_type c) {

    typename hashes::poseidon_block_type hash_result = hashes::poseidon(a + b, b + c, a * b + c);

    return hash_result;
}
