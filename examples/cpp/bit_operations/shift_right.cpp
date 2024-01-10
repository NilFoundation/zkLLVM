#include <cstdint>

constexpr uint32_t shift = 5;

[[circuit]] uint32_t lshr(uint32_t a) {

    return a >> shift; // lshr
}
