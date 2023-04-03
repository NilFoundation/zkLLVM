[[circuit]] __zkllvm_curve_curve25519 verify_eddsa_signature (
    __zkllvm_curve_curve25519 R,
    __zkllvm_field_curve25519_scalar s,
    __zkllvm_curve_curve25519 pk,
    __zkllvm_field_curve25519_base M) {
        scalar_range(s); // 
        check_ec(R); // 
        check_ec(pk);

        vector k_vec = sha512(Rx, Ry, pkx, pky, M);
        var k = reduction(k_vec)

        return ((s*B) - (R + (pk*k)));

    }



