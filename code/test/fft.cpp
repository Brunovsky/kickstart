#include "test_utils.hpp"
#include "../numeric/fft.hpp"
#include "../lib/anynum.hpp"

void stress_test_fft_multiply() {
    constexpr int N = 1000;
    constexpr int V = 1000;
    intd distn(0, N);
    int errors = 0;
    static_assert(1LL * N * V * V <= INT_MAX);

    LOOP_FOR_DURATION_OR_RUNS_TRACKED (4s, now, 100'000, runs) {
        print_time(now, 4s, 40ms, "stress test fft multiply");

        int A = distn(mt), B = distn(mt);
        auto a = int_gen<int>(A, -V, V);
        auto b = int_gen<int>(B, -V, V);
        auto c = fft::fft_multiply(a, b);
        auto d = fft::naive_multiply(a, b);

        errors += c != d;
    }

    if (errors > 0) {
        double percent = 100.0 * errors / runs;
        printcl("{} ({:.1f}%) ERRORS (N<={},|V|<={})\n", errors, percent, N, V);
    }
}

void stress_test_fft_square() {
    constexpr int N = 1000;
    constexpr int V = 1000;
    intd distn(0, N);
    int errors = 0;
    static_assert(1LL * N * V * V <= INT_MAX);

    LOOP_FOR_DURATION_OR_RUNS_TRACKED (4s, now, 100'000, runs) {
        print_time(now, 4s, 40ms, "stress test fft square");

        int A = distn(mt);
        auto a = int_gen<int>(A, -V, V);
        auto c = fft::fft_square(a);
        auto d = fft::naive_multiply(a, a);

        errors += c != d;
    }

    if (errors > 0) {
        double percent = 100.0 * errors / runs;
        printcl("{} ({:.1f}%) ERRORS (N<={},|V|<={})\n", errors, percent, N, V);
    }
}

void speed_test_fft_multiply() {
    constexpr int V = 1000;
    vector<int> As = {10, 30, 100, 300, 1000, 3000, 10000, 30000};
    vector<int> Bs = {10, 30, 100, 300, 1000, 3000, 10000, 30000};

    int NAs = As.size(), NBs = Bs.size();
    mat<string> ffttime = make_table(As, Bs);
    mat<string> naivetime = make_table(As, Bs);
    mat<string> speedup = make_table(As, Bs);

    for (int ia = 1; ia <= NAs; ia++) {
        for (int ib = 1; ib <= NBs; ib++) {
            int A = As[ia - 1], B = Bs[ib - 1];

            START_ACC(fft);
            START_ACC(naive);

            LOOP_FOR_DURATION_OR_RUNS_TRACKED (1500ms, now, 10000, runs) {
                print_time(now, 1500ms, 50ms, "speed test fft multiply");

                auto a = uniform_gen_many<long>(A, -V, V);
                auto b = uniform_gen_many<long>(B, -V, V);

                START(fft);
                auto c = fft::fft_multiply(a, b);
                ADD_TIME(fft);

                START(naive);
                auto d = fft::naive_multiply(a, b);
                ADD_TIME(naive);
            }

            printcl(" -- fft multiply {}x{}\n", A, B);
            PRINT_TIME_MS(fft);
            PRINT_TIME_MS(naive);

            ffttime[ia][ib] = format("{:.1f}", 1.0 * TIME_US(fft) / runs);
            naivetime[ia][ib] = format("{:.1f}", 1.0 * TIME_US(naive) / runs);
            speedup[ia][ib] = format("{:.3f}", 1.0 * TIME_US(naive) / TIME_US(fft));
        }
    }

    print("fft:\n{}", ffttime);
    print("naive:\n{}", naivetime);
    print("speedup:\n{}", speedup);
}

template <typename Num>
void breakeven_test_fft_multiply(int V) {
    if (fft::INT8_BREAKEVEN || fft::INT4_BREAKEVEN || fft::DOUBLE_BREAKEVEN)
        return;

    vector<size_t> fft_time{1}, naive_time{1};

    // add -1 when fft is better, +1 when naive is better, run until delta is stop
    const int window = 40, stop = -30;
    int delta = 0, window_len = 0;
    int N = 0;

    while (delta > stop) {
        printcl("breakeven fft multiply N={} window={}", N, delta);
        N++;

        START_ACC(fft);
        START_ACC(naive);

        LOOP_FOR_DURATION_OR_RUNS (50ms, 10000) {
            auto a = uniform_gen_many<Num>(N, 0, V);
            auto b = uniform_gen_many<Num>(N, 0, V);

            START(fft);
            auto c = fft::fft_multiply(a, b);
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
    RUN_BLOCK(stress_test_fft_multiply());
    RUN_BLOCK(stress_test_fft_square());
    RUN_BLOCK(breakeven_test_fft_multiply<long>(500'000));
    RUN_BLOCK(breakeven_test_fft_multiply<int>(30'000));
    RUN_BLOCK(breakeven_test_fft_multiply<double>(300'000));
    RUN_BLOCK(speed_test_fft_multiply());
    return 0;
}
