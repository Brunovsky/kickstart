#include "test_utils.hpp"
#include "../numeric/fft.hpp"
#include "../lib/anynum.hpp"

void unit_test_ntt_split_mod() {
    vector<int> a = {123412, 315312, 644121};
    vector<int> b = {123512, 52319023, 123512};
    auto c0 = fft::ntt_multiply_mod(1'000'000'007, a, b);
    auto c1 = fft::naive_multiply_mod(1'000'000'007, a, b);
    auto d0 = fft::ntt_multiply_mod(1'479'118'951, a, b);
    auto d1 = fft::naive_multiply_mod(1'479'118'951, a, b);
    auto e0 = fft::ntt_square_mod(1'000'000'007, a);
    auto e1 = fft::naive_multiply_mod(1'000'000'007, a, a);
    print("c0: {}\n", c0);
    print("c1: {}\n", c1);
    print("d0: {}\n", d0);
    print("d1: {}\n", d1);
    print("e0: {}\n", e0);
    print("e1: {}\n", e1);
}

void stress_test_ntt_multiply_mod(int N = 10000) {
    constexpr int mod = 1'000'000'007;
    constexpr int V = 1'000'000'000;
    intd distn(0, N);
    int errors = 0;

    LOOP_FOR_DURATION_OR_RUNS_TRACKED (4s, now, 100'000, runs) {
        print_time(now, 4s, 40ms, "stress test ntt multiply mod");

        int A = distn(mt), B = distn(mt);
        auto a = int_gen<int>(A, 0, V);
        auto b = int_gen<int>(B, 0, V);
        auto c = fft::ntt_multiply_mod(mod, a, b);
        auto d = fft::naive_multiply_mod(mod, a, b);

        errors += c != d;
    }

    if (errors > 0) {
        double percent = 100.0 * errors / runs;
        printcl("{} ({:.1f}%) ERRORS (N<={},|V|<={})\n", errors, percent, N, V);
    }
}

void stress_test_ntt_square_mod(int N = 10000) {
    constexpr int mod = 1'000'000'007;
    constexpr int V = 1'000'000'000;
    intd distn(0, N);
    int errors = 0;

    LOOP_FOR_DURATION_OR_RUNS_TRACKED (4s, now, 100'000, runs) {
        print_time(now, 4s, 40ms, "stress test ntt square mod");

        int A = distn(mt);
        auto a = int_gen<int>(A, 0, V);
        auto c = fft::ntt_square_mod(mod, a);
        auto d = fft::naive_multiply_mod(mod, a, a);

        errors += c != d;
    }

    if (errors > 0) {
        double percent = 100.0 * errors / runs;
        printcl("{} ({:.1f}%) ERRORS (N<={},|V|<={})\n", errors, percent, N, V);
    }
}

void breakeven_test_ntt_multiply_mod() {
    constexpr int mod = 1'000'000'007;
    constexpr int V = 800'000'000;
    vector<size_t> fft_time{1}, naive_time{1};

    // add -1 when fft is better, +1 when naive is better, run until delta is stop
    const int window = 40, stop = -30;
    int delta = 0, window_len = 0;
    int N = 0;

    while (delta > stop) {
        printcl("breakeven ntt multiply mod N={} window={}", N, delta);
        N++;

        START_ACC(fft);
        START_ACC(naive);

        LOOP_FOR_DURATION_OR_RUNS (30ms, 10000) {
            auto a = uniform_gen_many<int>(N, 0, V);
            auto b = uniform_gen_many<int>(N, 0, V);

            START(fft);
            auto c = fft::ntt_multiply_mod(mod, a, b);
            ADD_TIME(fft);

            START(naive);
            auto d = fft::naive_multiply_mod(mod, a, b);
            ADD_TIME(naive);
        }

        fft_time.push_back(TIME_US(fft));
        naive_time.push_back(TIME_US(naive));

        delta += fft_time[N] < naive_time[N] ? -1 : 1;
        window_len++;
        if (window_len > window) {
            delta -= fft_time[N - window] < naive_time[N - window] ? -1 : 1;
            window_len--;
        }
    }

    printcl("breakeven: N={}\n", N - window / 2);
}

void speed_test_ntt_split_multiply() {
    using num = modnum<998244353>;
    constexpr int V = 1000;
    vector<int> As = {10, 30, 100, 300, 1000, 3000, 10000, 30000};
    vector<int> Bs = {10, 30, 100, 300, 1000, 3000, 10000, 30000};

    int NAs = As.size(), NBs = Bs.size();
    mat<string> splittime = make_table(As, Bs);
    mat<string> ntttime = make_table(As, Bs);
    mat<string> speedup = make_table(As, Bs);

    for (int ia = 1; ia <= NAs; ia++) {
        for (int ib = 1; ib <= NBs; ib++) {
            int A = As[ia - 1], B = Bs[ib - 1];

            START_ACC(ntt_split);
            START_ACC(ntt);
            START_ACC(naive);

            LOOP_FOR_DURATION_OR_RUNS_TRACKED (2000ms, now, 10000, runs) {
                print_time(now, 2000ms, 50ms, "speed test fft multiply");

                auto a = uniform_gen_many<int>(A, -V, V);
                auto b = uniform_gen_many<int>(B, -V, V);
                vector<num> c(begin(a), end(a));
                vector<num> d(begin(b), end(b));

                START(ntt_split);
                auto x = fft::ntt_multiply_mod(998244353, a, b);
                ADD_TIME(ntt_split);

                START(ntt);
                auto y = fft::ntt_multiply(c, d);
                ADD_TIME(ntt);

                START(naive);
                auto z = fft::naive_multiply_mod(998244353, a, b);
                ADD_TIME(naive);
            }

            printcl(" -- ntt split multiply speed test {}x{} ({} runs)\n", A, B, runs);
            PRINT_TIME_MS(ntt_split);
            PRINT_TIME_MS(ntt);
            PRINT_TIME_MS(naive);

            splittime[ia][ib] = format("{:.1f}", EACH_US(ntt_split, runs));
            ntttime[ia][ib] = format("{:.1f}", EACH_US(ntt, runs));
            speedup[ia][ib] = format("{:.3f}", 1.0 * TIME_US(ntt) / TIME_US(ntt_split));
        }
    }

    print("splittime:\n{}", splittime);
    print("ntttime:\n{}", ntttime);
    print("speedup:\n{}", speedup);
}

int main() {
    RUN_BLOCK(unit_test_ntt_split_mod());
    RUN_SHORT(stress_test_ntt_square_mod());
    RUN_SHORT(stress_test_ntt_multiply_mod());
    RUN_SHORT(speed_test_ntt_split_multiply());
    RUN_BLOCK(breakeven_test_ntt_multiply_mod());
    return 0;
}
