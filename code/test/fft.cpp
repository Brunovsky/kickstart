#include "test_utils.hpp"
#include "../numeric/fft.hpp"
#include "../numeric/ntt.hpp"
#include "../lib/anynum.hpp"

using num = modnum<998244353>;

namespace std {

template <typename T>
string to_string(complex<T> c) {
    return format("({:5.1f} {:5.1f}) ", c.real(), c.imag());
}

template <typename T>
ostream& operator<<(ostream& out, complex<T> c) {
    return out << to_string(c);
}

} // namespace std

inline namespace detail {

template <typename T>
auto naive_multiply(const vector<T>& a, const vector<T>& b) {
    int A = a.size(), B = b.size(), S = A && B ? A + B - 1 : 0;
    vector<T> c(S);
    for (int i = 0; i < A; i++) {
        for (int j = 0; j < B; j++) {
            c[i + j] += a[i] * b[j];
        }
    }
    return c;
}

template <typename T>
auto naive_square(const vector<T>& a) {
    int A = a.size(), S = A ? 2 * A - 1 : 0;
    vector<T> c(S);
    for (int i = 0; i < A; i++) {
        for (int j = 0; j < A; j++) {
            c[i + j] += a[i] * a[j];
        }
    }
    return c;
}

template <typename D>
vector<complex<D>> complex_gen(int N, D real_min, D real_max, D imag_min, D imag_max) {
    vector<complex<D>> v(N);
    reald xd(real_min, real_max), yd(imag_min, imag_max);
    for (int i = 0; i < N; i++) {
        v[i] = complex<D>(xd(mt), yd(mt));
    }
    return v;
}

} // namespace detail

inline namespace stress_testing_fft {

void stress_test_next_two() {
    for (int n = 0; n <= 1000; n++) {
        int Q = 1, b = 0;
        while (Q < n) {
            Q <<= 1, b++;
        }
        assert(fft::next_two(n) == b);
    }
}

template <typename I>
void stress_test_fft_multiply(int N = 1000, int V = 1000) {
    intd distn(0, N);
    int errors = 0;

    LOOP_FOR_DURATION_OR_RUNS_TRACKED(4s, now, 100'000, runs) {
        print_time(now, 4s, 40ms, "stress test fft multiply");

        int A = distn(mt), B = distn(mt);
        auto a = int_gen<I>(A, -V, V);
        auto b = int_gen<I>(B, -V, V);
        auto c = fft::fft_multiply(a, b);
        auto d = naive_multiply(a, b);

        errors += c != d;
    }

    double percent = 1.0 * errors / runs;
    printcl("{} ({:.1f}%) ERRORS (N<={},|V|<={})\n", errors, percent, N, V);
}

template <typename I>
void stress_test_fft_square(int N = 1000, int V = 1000) {
    intd distn(0, N);
    int errors = 0;

    LOOP_FOR_DURATION_OR_RUNS_TRACKED(4s, now, 100'000, runs) {
        print_time(now, 4s, 40ms, "stress test fft square");

        int A = distn(mt);
        auto a = int_gen<I>(A, -V, V);
        auto c = fft::fft_square(a);
        auto d = naive_square(a);

        errors += c != d;
    }

    double percent = 1.0 * errors / runs;
    printcl("{} ({:.1f}%) ERRORS (N<={},|V|<={})\n", errors, percent, N, V);
}

void stress_test_ntt_multiply(int N = 1000, int V = 1000) {
    intd distn(0, N);
    int errors = 0;

    LOOP_FOR_DURATION_OR_RUNS_TRACKED(4s, now, 100'000, runs) {
        print_time(now, 4s, 40ms, "stress test ntt multiply");

        int A = distn(mt), B = distn(mt);
        auto a = uniform_gen_many<int, num>(A, 0, V);
        auto b = uniform_gen_many<int, num>(B, 0, V);
        auto c = fft::ntt_multiply(a, b);
        auto d = naive_multiply(a, b);

        errors += c != d;
    }

    double percent = 1.0 * errors / runs;
    printcl("{} ({:.1f}%) ERRORS (N<={},|V|<={})\n", errors, percent, N, V);
}

} // namespace stress_testing_fft

inline namespace speed_testing_fft {

template <typename I = long>
void speed_test_fft_multiply(int V = 1000) {
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

            LOOP_FOR_DURATION_OR_RUNS_TRACKED(1s, now, 10000, runs) {
                print_time(now, 1s, 50ms, "speed test fft multiply");

                auto a = uniform_gen_many<I>(A, -V, V);
                auto b = uniform_gen_many<I>(B, -V, V);

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

template <typename T>
void breakeven_test_fft_square(int V = 100) {
    vector<size_t> fft_time{1}, naive_time{1};

    // add -1 when fft is better, +1 when naive is better, run until delta is stop
    const int window = 40, stop = -30;
    int delta = 0, window_len = 0;
    int N = 0;

    while (delta > stop) {
        N++;
        auto ratio = 1.0 * fft_time[N - 1] / naive_time[N - 1];

        START_ACC(fft);
        START_ACC(naive);

        LOOP_FOR_DURATION_OR_RUNS(30ms, 10000) {
            auto a = uniform_gen_many<T>(N, -V, V);
            auto b = uniform_gen_many<T>(N, -V, V);

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

template <typename T>
void breakeven_test_ntt_square(int V = 10000) {
    vector<size_t> fft_time{1}, naive_time{1};

    // add -1 when fft is better, +1 when naive is better, run until delta is stop
    const int window = 40, stop = -30;
    int delta = 0, window_len = 0;
    int N = 0;

    while (delta > stop) {
        N++;
        auto ratio = 1.0 * fft_time[N - 1] / naive_time[N - 1];

        START_ACC(fft);
        START_ACC(naive);

        LOOP_FOR_DURATION_OR_RUNS(30ms, 10000) {
            auto a = uniform_gen_many<int, num>(N, 0, V);
            auto b = uniform_gen_many<int, num>(N, 0, V);

            START(fft);
            auto c = fft::ntt_multiply(a, b);
            ADD_TIME(fft);

            START(naive);
            auto d = naive_multiply(a, b);
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

void breakeven_test_ntt_split_square(int mod = 1'000'000'007, int V = 1000000) {
    vector<size_t> fft_time{1}, naive_time{1};

    // add -1 when fft is better, +1 when naive is better, run until delta is stop
    const int window = 40, stop = -30;
    int delta = 0, window_len = 0;
    int N = 0;

    while (delta > stop) {
        N++;
        auto ratio = 1.0 * fft_time[N - 1] / naive_time[N - 1];

        START_ACC(fft);
        START_ACC(naive);

        LOOP_FOR_DURATION_OR_RUNS(30ms, 10000) {
            auto a = uniform_gen_many<int>(N, 0, V);
            auto b = uniform_gen_many<int>(N, 0, V);

            START(fft);
            auto c = fft::ntt_split_multiply(mod, a, b);
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

} // namespace speed_testing_fft

inline namespace unit_testing_ntt {

void unit_test_ntt() {
    vector<num> a = {123412, 315312, 644121};
    vector<num> b = {123512, 52319023, 123512};
    auto c = fft::ntt_multiply(a, b);
    auto d = naive_multiply(a, b);
    print("c: {}\n", c);
    print("d: {}\n", d);
}

void unit_test_ntt_split() {
    vector<int> a = {123412, 315312, 644121};
    vector<int> b = {123512, 52319023, 123512};
    auto c0 = fft::ntt_split_multiply(1'000'000'007, a, b);
    auto d0 = fft::naive_multiply_mod(1'000'000'007, a, b);
    auto c1 = fft::ntt_split_multiply(1'479'118'951, a, b);
    auto d1 = fft::naive_multiply_mod(1'479'118'951, a, b);
    print("c0: {}\n", c0);
    print("d0: {}\n", d0);
    print("c1: {}\n", c1);
    print("d1: {}\n", d1);
}

} // namespace unit_testing_ntt

int main() {
    RUN_SHORT(stress_test_next_two());
    RUN_SHORT(unit_test_ntt());
    RUN_SHORT(unit_test_ntt_split());
    RUN_BLOCK(breakeven_test_ntt_split_square());
    RUN_BLOCK(breakeven_test_fft_square<int>());
    RUN_BLOCK(breakeven_test_fft_square<long>());
    RUN_BLOCK(breakeven_test_fft_square<double>());
    RUN_BLOCK(breakeven_test_ntt_square<num>());
    RUN_SHORT(stress_test_fft_multiply<int>());
    RUN_SHORT(stress_test_fft_multiply<long>());
    RUN_SHORT(stress_test_fft_square<int>());
    RUN_SHORT(stress_test_fft_square<long>());
    RUN_SHORT(stress_test_ntt_multiply());
    RUN_BLOCK(speed_test_fft_multiply<long>());
    RUN_BLOCK(speed_test_fft_multiply<double>());
    return 0;
}
