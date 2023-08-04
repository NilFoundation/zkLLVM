#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] uint32_t remainder(uint32_t a, uint32_t b) {

    return a % b;
}