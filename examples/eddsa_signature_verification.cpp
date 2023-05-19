#include <nil/crypto3/algebra/fields/curve25519/base_field.hpp>
#include <nil/crypto3/algebra/fields/curve25519/scalar_field.hpp>
#include <nil/crypto3/algebra/curves/ed25519.hpp>

using ed25519_element = typename nil::crypto3::algebra::curves::ed25519::template g1_type<>::value_type;

[[circuit]] bool verify_eddsa_signature (
    ed25519_element R,
    __zkllvm_field_curve25519_scalar s,
    ed25519_element pk,
    ed25519_element M) {

        ed25519_element B = ed25519_element::one();
        __zkllvm_field_curve25519_scalar k = __builtin_assigner_sha2_512_curve25519(R.data, pk.data, M.data);

        return (B*s - (R + (pk*k))).is_zero();

    }