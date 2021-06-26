#include "test_utils.hpp"
#include "../numeric/fft.hpp"
#include "../lib/anynum.hpp"

template <typename Num = int>
void stress_test_fft_multiply(int N = 1000, Num V = 1000) {
    intd distn(0, N);
    int errors = 0;

    LOOP_FOR_DURATION_OR_RUNS_TRACKED (4s, now, 100'000, runs) {
        print_time(now, 4s, "stress test fft multiply");

        int A = distn(mt), B = distn(mt);
        auto a = uniform_gen_many<Num>(A, -V, V);
        auto b = uniform_gen_many<Num>(B, -V, V);

        auto c = fft::fft_multiply(a, b);
        auto d = fft::naive_multiply(a, b);

        errors += c != d;
    }

    if (errors > 0) {
        double percent = 100.0 * errors / runs;
        printcl("{} ({:.1f}%) ERRORS (N<={},|V|<={})\n", errors, percent, N, V);
    }
}

template <typename Num = int>
void speed_test_fft_multiply(int V = 1000) {
    constexpr long naive_threshold = 50'000'000;
    vector<int> As = {10, 30, 100, 300, 1000, 3000, 10000, 30000, 100000, 300000};
    vector<int> Bs = {10, 30, 100, 300, 1000, 3000, 10000, 30000, 100000, 300000};
    const auto duration = 30000ms / (As.size() * Bs.size());

    map<tuple<int, int, string>, string> table;

    for (int A : As) {
        for (int B : Bs) {
            bool run_naive = 1LL * A * B <= naive_threshold;
            START_ACC2(fft, naive);

            LOOP_FOR_DURATION_OR_RUNS_TRACKED (duration, now, 10000, runs) {
                print_time(now, duration, "speed test fft multiply A,B={},{}", A, B);

                auto a = uniform_gen_many<Num>(A, -V, V);
                auto b = uniform_gen_many<Num>(B, -V, V);

                START(fft);
                auto c = fft::fft_multiply(a, b);
                ADD_TIME(fft);

                if (run_naive) {
                    START(naive);
                    auto d = fft::naive_multiply(a, b);
                    ADD_TIME(naive);
                }
            }

            table[{A, B, "fft"}] = FORMAT_EACH(fft, runs);
            table[{A, B, "naive"}] = FORMAT_EACH(naive, runs);
        }
    }

    print_time_table(table, "FFT");
}

template <typename Num>
void breakeven_test_fft_multiply(int V) {
    if (fft::INT8_BREAKEVEN > 0 || fft::INT4_BREAKEVEN > 0 || fft::DOUBLE_BREAKEVEN > 0)
        return;

    vector<size_t> fft_time{1}, naive_time{1};

    // add -1 when fft is better, +1 when naive is better, run until delta is stop
    const int window = 40, stop = -30;
    int delta = 0, window_len = 0;
    int N = 0;

    while (delta > stop) {
        printcl("breakeven fft multiply N={} window={}", N, delta);
        N++;

        START_ACC2(fft, naive);

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
    RUN_BLOCK(breakeven_test_fft_multiply<int>(30'000));
    RUN_BLOCK(breakeven_test_fft_multiply<long>(500'000));
    RUN_BLOCK(breakeven_test_fft_multiply<double>(300'000));
    RUN_BLOCK(speed_test_fft_multiply());
    return 0;
}
