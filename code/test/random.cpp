#include "../random.hpp"

#include "../numeric/math.hpp"
#include "test_utils.hpp"

// *****

/**
 * Test normality of count distribution
 */
void verify_normality(vector<int>& v) {
    int n = v.size();
    int m = *min_element(begin(v), end(v));
    int M = *max_element(begin(v), end(v));
    long sum = accumulate(begin(v), end(v), 0L);
    double mean = 1.0 * sum / n;
    double variance = 0;
    for (int k : v) {
        variance += (k - mean) * (k - mean);
    }
    variance /= n;
    double rho = sqrt(variance);
    unordered_map<int, int> within; // count numbers within k standard deviations
    for (int k : v) {
        within[int(abs(k - mean) / rho)]++;
    }
    for (int i = 1; i <= 5; i++) {
        within[i] += within[i - 1];
    }

    clear_line();
    print("===== NORMALITY TEST =====\n");
    print("sum: {}\n", sum);
    print("min: {}  ({:+.2f})\n", m, (m - mean) / rho);
    print("max: {}  ({:+.2f})\n", M, (M - mean) / rho);
    print("mean: {:.2f}\n", mean);
    print("rho:  {:.2f}\n", rho);
    print("within 1 rho: {:5.2f}%\n", 100.0 * within[0] / n);
    print("within 2 rho: {:5.2f}%\n", 100.0 * within[1] / n);
    print("within 3 rho: {:5.2f}%\n", 100.0 * within[2] / n);
    print("within 4 rho: {:5.2f}%\n", 100.0 * within[3] / n);
    print("(remember, expected: 68.27 - 95.45 - 99.73)\n");
    assert(m > 0.5 * mean);
    assert(M < 2.0 * mean);
}

void stress_test_vec_sample(int T = 70000, int n = 93439, int k = 173) {
    int start = 87632;
    vector<int> univ(n);
    iota(begin(univ), end(univ), start);
    shuffle(begin(univ), end(univ), mt);
    vector<int> cnt(n, 0);

    for (int t = 0; t < T; t++) {
        print_progress(t, T, "stress test vec_sample");
        vector<int> sample = vec_sample(univ, k);
        for (int m : sample) {
            assert(start <= m && m < start + n);
            cnt[m - start]++;
        }
        assert(sample.size() == k);
    }
    verify_normality(cnt);
    print_ok("stress test vec sample");
}

void scaling_test_int_sample(long F = 5'000'000) {
    intd dista(0, 25000);

    vector<int> abs = {
        100, 1000, 10'000, 100'000, 1'000'000,
    };
    vector<long> sparse = {
        10, 40, 90, 400, 900, 4'000, 9'000, 40'000, 90'000, 400'000, 900'000,
    };
    vector<double> dense = {
        .01, .025, .05, .1, .2, .3, .5, .75, .9, .98,
    };

    int t = 0, T = size(abs) * (size(sparse) + size(dense));

    for (long k : sparse) {
        print(" int sample sparse test x{}\n", k);
        for (int n : abs) {
            print_progress(t++, T, "scaling test int sample");
            if (k > n)
                continue;
            int N = F / k;
            START(sampler);
            for (int i = 0; i < N; i++) {
                int a = dista(mt), b = a + n;
                int_sample(k, a, b);
            }
            TIME(sampler);
            print(" {:>8}ms -- {:>9.1f}us each -- {:>8} n -- sparse {:>8}\n",
                  TIME_MS(sampler), 1.0 * TIME_US(sampler) / N, n, k);
        }
    }
    for (double p : dense) {
        print(" int sample dense test p={:5.3f}\n", p);
        for (int n : abs) {
            print_progress(t++, T, "scaling test int sample");
            START(sampler);
            int N = ceil(F / (p * n));
            for (int i = 0; i < N; i++) {
                int a = dista(mt), b = a + n;
                int_sample_p(p, a, b);
            }
            TIME(sampler);
            print(" {:>8}ms -- {:>9.1f}us each -- {:>8} n -- dense {:>5.3f}\n",
                  TIME_MS(sampler), 1.0 * TIME_US(sampler) / N, n, p);
        }
    }

    print_ok("scaling test int sample");
}

void scaling_test_choose_sample(long F = 5'000'000) {
    intd dista(0, 25000);

    vector<int> abs_sparse = {
        100, 1000, 10'000, 100'000, 1'000'000,
    };
    vector<long> sparse = {
        10, 40, 90, 400, 900, 4'000, 9'000, 40'000, 90'000, 400'000, 900'000,
    };
    vector<int> abs_dense = {
        10, 40, 100, 300, 800, 2'000, 5'000,
    };
    vector<double> dense = {
        .01, .025, .05, .1, .2, .3, .5, .75, .9, .98,
    };

    int t = 0, T = size(abs_sparse) * size(sparse) + size(abs_dense) * size(dense);

    for (long k : sparse) {
        print(" choose sample sparse test x{}\n", k);
        for (int n : abs_sparse) {
            print_progress(t++, T, "scaling test choose sample");
            if (k > choose(n, 2))
                continue;
            int N = F / k;
            START(sampler);
            for (int i = 0; i < N; i++) {
                int a = dista(mt), b = a + n;
                choose_sample(k, a, b);
            }
            TIME(sampler);
            print(" {:>8}ms -- {:>9.1f}us each -- {:>8} n -- sparse {:>8}\n",
                  TIME_MS(sampler), 1.0 * TIME_US(sampler) / N, n, k);
        }
    }
    for (double p : dense) {
        print(" choose sample dense test p={:5.3f}\n", p);
        for (int n : abs_dense) {
            print_progress(t++, T, "scaling test choose sample");
            START(sampler);
            int N = max(1, int(F / (p * choose(n, 2))));
            for (int i = 0; i < N; i++) {
                int a = dista(mt), b = a + n;
                choose_sample_p(p, a, b);
            }
            TIME(sampler);
            print(" {:>8}ms -- {:>9.1f}us each -- {:>8} n -- dense {:>5.3f}\n",
                  TIME_MS(sampler), 1.0 * TIME_US(sampler) / N, n, p);
        }
    }

    print_ok("scaling test int sample");
}

void scaling_test_pair_sample(long F = 5'000'000) {
    intd dista(0, 25000), distc(0, 25000);

    vector<array<int, 2>> abs_sparse = {
        {100, 100}, {100, 500},  {300, 300},  {100, 3000},
        {500, 500}, {500, 1000}, {300, 2000}, {1000, 1000},
    };
    vector<long> sparse = {
        10, 40, 90, 400, 900, 4'000, 9'000, 40'000, 90'000, 400'000, 900'000,
    };
    vector<array<int, 2>> abs_dense = {
        {20, 20}, {30, 30}, {20, 50}, {10, 200}, {20, 100}, {50, 50},
    };
    vector<double> dense = {
        .01, .025, .05, .1, .2, .3, .5, .75, .9, .98,
    };

    int t = 0, T = size(abs_sparse) * size(sparse) + size(abs_dense) * size(dense);

    for (long k : sparse) {
        print(" pair sample sparse test x{}\n", k);
        for (auto [n, m] : abs_sparse) {
            print_progress(t++, T, "scaling test pair sample");
            if (k > n * m)
                continue;
            int N = F / k;
            START(sampler);
            for (int i = 0; i < N; i++) {
                int a = dista(mt), b = a + n, c = distc(mt), d = c + m;
                pair_sample(k, a, b, c, d);
            }
            TIME(sampler);
            print(" {:>8}ms -- {:>9.1f}us each -- {:>5}x{:<5} -- sparse {:>8}\n",
                  TIME_MS(sampler), 1.0 * TIME_US(sampler) / N, n, m, k);
        }
    }
    for (double p : dense) {
        print(" pair sample dense test p={:5.3f}\n", p);
        for (auto [n, m] : abs_dense) {
            print_progress(t++, T, "scaling test pair sample");
            START(sampler);
            int N = max(1, int(F / (p * n * m)));
            for (int i = 0; i < N; i++) {
                int a = dista(mt), b = a + n, c = distc(mt), d = c + m;
                pair_sample_p(p, a, b, c, d);
            }
            TIME(sampler);
            print(" {:>8}ms -- {:>9.1f}us each -- {:>5}x{:<5} -- dense {:>5.3f}\n",
                  TIME_MS(sampler), 1.0 * TIME_US(sampler) / N, n, m, p);
        }
    }

    print_ok("scaling test int sample");
}

int main() {
    stress_test_vec_sample();
    scaling_test_int_sample();
    scaling_test_pair_sample();
    scaling_test_choose_sample();
    return 0;
}
