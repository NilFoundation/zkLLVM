int pow_2(int a) {
    int res = 1;
    for (int i = 0; i < 2; ++i) {
        res *= a;
    }
    return res;
}

[[circuit]] int int_arithmetic_example(int a, int b) {
    int c = (a + b) * a + b * (a + b) * (a + b);
    const int constant = 77;
    return c * c * c * (b - a) + pow_2(a) + constant;
}
