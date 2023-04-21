#include<array>
#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

struct sponge_type {
    poseidon_block_type state;

    std::size_t state_count = 0;
    bool state_absorbed = true;

    constexpr static const std::size_t state_size = poseidon_component::state_size;

    void init() {
        state = {0, 0, 0};
    }

    void absorb(typename pallas::base_field_type::value_type absorbing_value) {
        if (this->state_absorbed) {
            if (this->state_count == poseidon_component::rate) {
                state = hashes::posseidon(state);
                state[0] = state[0] + absorbing_value;
                this->state_count = 1;
            } else {
                state[this->state_count] = state[this->state_count] + absorbing_value;
                this->state_count++;
            }
        } else {
            state[0] = state[0] + absorbing_value;
            this->state_absorbed = true;
            this->state_count = 1;
        }
    }

    typename pallas::base_field_type::value_type squeeze() {
        if (!this->state_absorbed) {    // state = squeezed
            if (this->state_count == poseidon_component::rate) {
                state = hashes::posseidon(state);
                this->state_count = 1;
                return this->state[0];
            } else {
                return this->state[this->state_count++];
            }
        } else {
            state = hashes::posseidon(state);

            this->state_absorbed = false;
            this->state_count = 1;
            return this->state[0];
        }
    }
}