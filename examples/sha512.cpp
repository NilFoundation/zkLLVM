
namespace nil{
    namespace crypto3{
        namespace algebra {
            struct bls12381{
                typedef field_bls12381b value_type;
            };
        }

        namespace hashes {
            typedef __attribute__((ext_vector_type(8))) typename algebra::bls12381::value_type sha512_state_type;
            typedef __attribute__((ext_vector_type(16))) typename algebra::bls12381::value_type sha512_input_words_type;

            sha512_state_type sha512(sha512_state_type input_state, sha512_input_words_type input_words);
        }
    }
}

using namespace nil::crypto3;

typename hashes::sha512_state_type sha512_example(
    hashes::sha512_state_type input_state, hashes::sha512_input_words_type input_words) {

    typename hashes::sha512_state_type hash_result = hashes::sha512(input_state, input_words);

    return hash_result;
}
