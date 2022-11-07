field_bls12381b arithmetics(field_bls12381b a, field_bls12381b b) {
    auto mul = a * b;
    return mul + a;
}
