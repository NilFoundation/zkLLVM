
namespace nil{
    namespace crypto3{
        namespace algebra {
            struct fields {
                typedef field_curve25519b curve25519_base_field;
            };
        }

        namespace hashes {
            typedef __attribute__((ext_vector_type(8))) typename algebra::fields::curve25519_base_field sha512_state_type;
            typedef __attribute__((ext_vector_type(16))) typename algebra::fields::curve25519_base_field sha512_input_words_type;

            sha512_state_type sha512(sha512_state_type input_state, sha512_input_words_type input_words);
        }
    }
}

using namespace nil::crypto3;

[[circuit]] typename hashes::sha512_state_type sha512_example(
    hashes::sha512_state_type input_state, hashes::sha512_input_words_type input_words) {

    typename hashes::sha512_state_type hash_result = hashes::sha512(input_state, input_words);

    return hash_result;
}
