field_bls12381b arithmetics(field_bls12381b a, field_bls12381b b) {
    if (a > b)
        return a * b;
    return a / b;
}
