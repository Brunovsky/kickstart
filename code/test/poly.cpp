#include "test_utils.hpp"
#include "../numeric/fft.hpp"
#include "../numeric/poly.hpp"
#include "../lib/anynum.hpp"

using num = modnum<998244353>;
using poly = vector<num>;
using namespace polymath;

auto polyroot(num val) { return poly{-val, 1}; }

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

void stress_test_division() {
    LOOP_FOR_DURATION_TRACKED_RUNS (2s, now, runs) {
        print_time(now, 2s, "stress test poly division ({} runs)", runs);

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

void speed_test_multieval() {
    const int max_N = 1 << 15;
    map<pair<string, int>, string> table;

    for (int N = 8; N <= max_N; N *= 2) {
        vector<num> x(N);
        iota(begin(x), end(x), 0);
        shuffle(begin(x), end(x), mt);

        auto tree = build_multieval_tree(x);

        START_ACC2(multieval, naive);

        LOOP_FOR_DURATION_OR_RUNS_TRACKED (1s, now, 1000, runs) {
            print_time(now, 1s, "speed test multipoint eval N={}", N);

            auto poly = uniform_gen_many<int, num>(N, 1, 100'000);

            START(multieval);
            auto v = multieval(poly, tree);
            ADD_TIME(multieval);

            START(naive);
            vector<num> ans(N);
            for (int i = 0; i < N; i++) {
                ans[i] = eval(poly, x[i]);
            }
            ADD_TIME(naive);
        }

        table[{"multieval", N}] = FORMAT_EACH(multieval, runs);
        table[{"naive", N}] = FORMAT_EACH(naive, runs);
    }

    print_time_table(table, "Multieval");
}

void speed_test_inverse_series() {
    const int max_N = 1 << 16;
    map<pair<string, int>, string> table;

    for (int N = 8; N <= max_N; N *= 2) {
        START_ACC(inverse);

        LOOP_FOR_DURATION_OR_RUNS_TRACKED (1s, now, 1000, runs) {
            print_time(now, 1s, "speed test inverse series N={}", N);

            auto a = uniform_gen_many<int, num>(N, 1, 100'000);

            START(inverse);
            auto b = inverse_series(a, N);
            ADD_TIME(inverse);
        }

        table[{"inverse", N}] = FORMAT_EACH(inverse, runs);
    }

    print_time_table(table, "Inverse series");
}

int main() {
    RUN_SHORT(unit_test_multieval());
    RUN_SHORT(unit_test_interpolate());
    RUN_BLOCK(stress_test_division());
    RUN_BLOCK(speed_test_multieval());
    RUN_BLOCK(speed_test_inverse_series());
    return 0;
}
