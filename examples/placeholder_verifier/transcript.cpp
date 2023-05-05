#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>

using namespace nil::crypto3;

struct transcript {
    typename hashes::sha2<256>::block_type state;

    transcript (typename hashes::sha2<256>::block_type init) {
        state = hash<hashes::sha2<256>>(init, init);
    }

    void append(typename hashes::sha2<256>::block_type chunk) {
        state = hash<hashes::sha2<256>>(state, chunk);
    }

    typename hashes::sha2<256>::block_type challenge () {
        state = hash<hashes::sha2<256>>(state, state);
        return state;
    }

};

[[circuit]] typename hashes::sha2<256>::block_type sha256_example(typename hashes::sha2<256>::block_type first_input_block,
                                                              typename hashes::sha2<256>::block_type second_input_block) {

    typename hashes::sha2<256>::block_type hash_result = hash<hashes::sha2<256>>(first_input_block, second_input_block);
    transcript transcript_test = transcript(hash_result);
    transcript_test.append(hash_result);
    hash_result = transcript_test.challenge();

    return hash_result;
}
