#include "test_utils.hpp"
#include "../numeric/fft.hpp"
#include "../numeric/poly.hpp"
#include "../numeric/poly_multieval.hpp"
#include "../lib/anynum.hpp"

using num = modnum<998244353>;
using poly = vector<num>;
using namespace polymath;

auto polyroot(num val) { return poly{-val, 1}; }

inline namespace unit_testing_poly {

void unit_test_1() {
    poly a = {1, 3, 4, 5};
    poly b = {2, 5, 3};

    assert(a + b == poly({3, 8, 7, 5}));
    assert(deriv(a) == poly({3, 8, 15}));
    assert(integr(deriv(a), a[0]) == a);
    assert(integr(deriv(b), b[0]) == b);

    assert(num(2) * a == poly({2, 6, 8, 10}));

    poly ai = inverse_series(a, 16);
    print("inverse_series(a, 16): {}\n", ai);
    print("inverse_series(a, 16) * a: {}\n", a * ai);

    poly c = {1, 1};
    assert(binpow(c, 5) == poly({1, 5, 10, 10, 5, 1}));

    poly d = {1, 4, 6, 4, 1}; // (x+1)^4
    poly x1 = {1, 1};         // x + 1
    assert(d / x1 == poly({1, 3, 3, 1}));
    assert(d / x1 / x1 == poly({1, 2, 1}));
    assert(d / x1 / x1 / x1 == poly({1, 1}));
    assert(d % x1 == poly());
}

void unit_test_2() {
    poly a = {7, 6, 5, 4, 3, 2, 1};
    poly b = {4, 2, 3, 1};
    poly d = a / b;
    poly r = a - b * d;
    print("d: {}\n", d);
    print("r: {}\n", r);
    print("bd + r: {}\n", b * d + r);
    print("gcd(a, b) = {}\n", gcd(a, b));

    poly u(1, 1), v(1, 1), g(1, 1);
    for (int root : {1, 2, 3, 4, 5, 6})
        u *= polyroot(root);
    for (int root : {2, 6, 7, 8})
        v *= polyroot(root);
    for (int root : {2, 6})
        g *= polyroot(root);

    print("gcd(u,v) = {}\n", gcd(u, v));
    print("g = {}\n", g);
    print("resultant(u,v) = {}\n", resultant(u, v));

    print("u(2) = {}\n", eval(u, num(2)));
    print("u(7) = {}\n", eval(u, num(7)));
    print("u(9) = {}\n", eval(u, num(9)));
    print("b(3) = {}\n", eval(b, num(3)));
}

void unit_test_multieval() {
    constexpr int N = 23;
    vector<num> x(N);
    iota(begin(x), end(x), 0);

    poly a = {1, 3, 3, 1}; // (x+1)^3

    auto val = multieval(a, x);
    for (int i = 0; i < N; i++) {
        assert(val[i] == eval(a, x[i]));
    }
}

void unit_test_interpolate() {
    constexpr int N = 6;
    vector<num> x(N);
    iota(begin(x), end(x), 1);

    poly a = {1, 4, 9, 16, 25, 36};
    print("interpolate({}) = {}\n", a, interpolate(x, a));

    poly b = {1, 8, 27, 64, 125, 216};
    print("interpolate({}) = {}\n", b, interpolate(x, b));
}

} // namespace unit_testing_poly

inline namespace stress_testing_poly {

void stress_test_division() {
    LOOP_FOR_DURATION_TRACKED_RUNS (2s, now, runs) {
        print_time(now, 2s, 50ms, "stress test poly division ({} runs)", runs);

        auto [B, A] = different(10, 500);
        auto a = uniform_gen_many<int, num>(A, 1, 10000);
        auto b = uniform_gen_many<int, num>(B, 1, 10000);

        auto [d, r] = division_with_remainder(a, b);
        auto c = b * d + r;

        assert(int(d.size()) == A - B + 1);
        assert(int(r.size()) < B);
        assert(a == c);
    }
}

} // namespace stress_testing_poly

inline namespace speed_testing_poly {

void speed_test_multieval() {
    const int max_N = 1 << 15;

    for (int N = 8; N <= max_N; N *= 2) {
        vector<num> x(N);
        iota(begin(x), end(x), 0);
        shuffle(begin(x), end(x), mt);

        auto tree = build_multieval_tree(x);

        START_ACC(multieval);
        START_ACC(naive);

        LOOP_FOR_DURATION_OR_RUNS_TRACKED (1s, now, 1000, runs) {
            print_time(now, 1s, 50ms, "speed test multipoint eval");

            auto poly = uniform_gen_many<int, num>(N, 1, 100'000);

            START(multieval);
            auto v = multieval(poly, tree);
            ADD_TIME(multieval);

            START(naive);
            vector<num> ans(N);
            for (int i = 0; i < N; i++)
                ans[i] = eval(poly, x[i]);
            ADD_TIME(naive);
        }

        printcl("multipoint eval N={}\n", N);
        PRINT_EACH_MS(multieval, runs);
        PRINT_EACH_MS(naive, runs);
    }
}

void speed_test_inverse_series() {
    const int max_N = 1 << 16;

    for (int N = 8; N <= max_N; N *= 2) {
        START_ACC(inverse);

        LOOP_FOR_DURATION_OR_RUNS_TRACKED (1s, now, 1000, runs) {
            print_time(now, 1s, 50ms, "speed test inverse series");

            auto a = uniform_gen_many<int, num>(N, 1, 100'000);

            START(inverse);
            auto b = inverse_series(a, N);
            ADD_TIME(inverse);
        }

        printcl("inverse series N={}\n", N);
        PRINT_EACH_MS(inverse, runs);
    }
}

} // namespace speed_testing_poly

int main() {
    RUN_SHORT(unit_test_1());
    RUN_SHORT(unit_test_2());
    RUN_SHORT(unit_test_multieval());
    RUN_SHORT(unit_test_interpolate());
    RUN_BLOCK(stress_test_division());
    // RUN_BLOCK(speed_test_multieval());
    // RUN_BLOCK(speed_test_inverse_series());
    return 0;
}
