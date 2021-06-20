#include "test_utils.hpp"
#include "../numeric/fft.hpp"
#include "../lib/anynum.hpp"

void stress_test_fft_split_multiply() {
    constexpr int N = 1000;
    constexpr int V = 1000000;
    intd distn(0, N);
    int errors = 0;
    static_assert(1LL * N * V * V <= LONG_MAX / 8);

    LOOP_FOR_DURATION_OR_RUNS_TRACKED (4s, now, 100'000, runs) {
        print_time(now, 4s, "stress test fft split multiply");

        int A = distn(mt), B = distn(mt);
        auto a = rands_unif<long>(A, -V, V);
        auto b = rands_unif<long>(B, -V, V);
        auto c = fft::fft_split_multiply(a, b);
        auto d = fft::naive_multiply(a, b);

        errors += c != d;
    }

    if (errors > 0) {
        double percent = 100.0 * errors / runs;
        printcl("{} ({:.1f}%) ERRORS (N<={},|V|<={})\n", errors, percent, N, V);
    }
}

void breakeven_test_fft_split_multiply() {
    if (fft::SPLIT_BREAKEVEN)
        return;

    vector<size_t> fft_time{1}, naive_time{1};

    // add -1 when fft is better, +1 when naive is better, run until delta is stop
    const int window = 40, stop = -30;
    int delta = 0, window_len = 0;
    int N = 0;

    while (delta > stop) {
        printcl("breakeven fft split multiply N={} window={}", N, delta);
        N++;

        START_ACC(fft);
        START_ACC(naive);

        LOOP_FOR_DURATION_OR_RUNS (30ms, 10000) {
            auto a = uniform_gen_many<long>(N, 500'000, 10'000'000);
            auto b = uniform_gen_many<long>(N, 500'000, 10'000'000);

            START(fft);
            auto c = fft::fft_split_multiply(a, b);
            ADD_TIME(fft);

            START(naive);
            auto d = fft::naive_multiply(a, b);
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

int main() {
    RUN_BLOCK(stress_test_fft_split_multiply());
    RUN_BLOCK(breakeven_test_fft_split_multiply());
    return 0;
}
