#include "test_utils.hpp"
#include "../numeric/fft.hpp"
#include "../lib/anynum.hpp"

void unit_test_with_modnum() {
    using num = modnum<1'000'000'007>;
    vector<num> a = {123412, 315312, 644121};
    vector<num> b = {123512, 52319023, 123512};
    auto c0 = fft::fft_multiply(a, b);
    auto c1 = fft::naive_multiply(a, b);
    print("c0: {}\n", c0);
    print("c1: {}\n", c1);
}

void unit_test_fft_multiply_mod() {
    vector<int> a = {123412, 315312, 644121};
    vector<int> b = {123512, 52319023, 123512};
    auto c0 = fft::fft_multiply(1'000'000'007, a, b);
    auto c1 = fft::naive_multiply(1'000'000'007, a, b);
    auto d0 = fft::fft_multiply(1'479'118'951, a, b);
    auto d1 = fft::naive_multiply(1'479'118'951, a, b);
    print("c0: {}\n", c0);
    print("c1: {}\n", c1);
    print("d0: {}\n", d0);
    print("d1: {}\n", d1);
}

void stress_test_fft_multiply_mod(int N = 10000) {
    constexpr int mod = 1'000'000'007;
    constexpr int V = 1'000'000'000;
    intd distn(0, N);
    int errors = 0;

    LOOP_FOR_DURATION_OR_RUNS_TRACKED (4s, now, 100'000, runs) {
        print_time(now, 4s, "stress test ntt multiply mod");

        int A = distn(mt), B = distn(mt);
        auto a = int_gen<int>(A, 0, V);
        auto b = int_gen<int>(B, 0, V);
        auto c = fft::fft_multiply(mod, a, b);
        auto d = fft::naive_multiply(mod, a, b);

        errors += c != d;
    }

    if (errors > 0) {
        double percent = 100.0 * errors / runs;
        printcl("{} ({:.1f}%) ERRORS (N<={},|V|<={})\n", errors, percent, N, V);
    }
}

void breakeven_test_fft_multiply_mod() {
    if (fft::SPLITMOD_BREAKEVEN)
        return;

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
            auto c = fft::fft_multiply(mod, a, b);
            ADD_TIME(fft);

            START(naive);
            auto d = fft::naive_multiply(mod, a, b);
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

void speed_test_fft_multiply_mod() {
    using num = modnum<998244353>;
    constexpr int V = 900'000'000;
    vector<int> As = {10, 30, 100, 300, 1000, 3000, 10000, 30000, 100000, 300000};
    vector<int> Bs = {10, 30, 100, 300, 1000, 3000, 10000, 30000, 100000, 300000};
    const auto duration = 30000ms / (As.size() * Bs.size());
    map<tuple<int, int, string>, string> table;

    for (int A : As) {
        for (int B : Bs) {
            START_ACC2(ntt_split, ntt);

            LOOP_FOR_DURATION_OR_RUNS_TRACKED (duration, now, 10000, runs) {
                print_time(now, duration, "speed test fft multiply A,B={},{}", A, B);

                auto a = uniform_gen_many<int>(A, -V, V);
                auto b = uniform_gen_many<int>(B, -V, V);
                vector<num> c(begin(a), end(a));
                vector<num> d(begin(b), end(b));

                START(ntt_split);
                auto x = fft::fft_multiply(998244353, a, b);
                ADD_TIME(ntt_split);

                START(ntt);
                auto y = fft::fft_multiply(c, d);
                ADD_TIME(ntt);
            }

            table[{A, B, "ntt"}] = FORMAT_EACH(ntt, runs);
            table[{A, B, "split"}] = FORMAT_EACH(ntt_split, runs);
        }
    }

    print_time_table(table, "FFT time");
}

int main() {
    RUN_SHORT(unit_test_with_modnum());
    RUN_SHORT(unit_test_fft_multiply_mod());
    RUN_SHORT(stress_test_fft_multiply_mod());
    RUN_BLOCK(breakeven_test_fft_multiply_mod());
    RUN_SHORT(speed_test_fft_multiply_mod());
    return 0;
}
