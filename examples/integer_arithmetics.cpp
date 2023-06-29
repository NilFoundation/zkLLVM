int pow(int a, int n) {
    if (n == 0)
        return 1;

    int res = 1;
    for (int i = 0; i < n; ++i) {
        res *= a;
    }
    return res;
}

[[circuit]] int int_arithmetic_example(int a, int b) {
    int c = (a + b) * a + b * (a + b) * (a + b);
    const int constant = 77;
    return c * c * c * (b - a) + pow(a, 2) + constant;
}
