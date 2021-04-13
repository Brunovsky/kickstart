#include "../random.hpp"

#include "../formatting.hpp"
#include "../hash.hpp"
#include "../numeric/math.hpp"
#include "../numeric/partitions.hpp"
#include "test_utils.hpp"

// *****

using vi = vector<int>;
unordered_map<vi, vector<vi>> partitions_memo;

const vector<vi>& generate_all_partitions(int n, int k, int m, int M = INT_MAX) {
    M = min(M, n);
    if (partitions_memo.count({n, k, m, M}))
        return partitions_memo[{n, k, m, M}];
    if (n < m * k || M * k < n || !k)
        return partitions_memo[{n, k, m, M}] = {};
    if (k == 1)
        return partitions_memo[{n, k, m, M}] = {{n}};

    vector<vi> pts;
    for (int x = min(M, n); x >= m; x--) {
        auto subpts = generate_all_partitions(n - x, k - 1, m, x);
        for (auto& pt : subpts)
            pt.push_back(x);
        pts.insert(end(pts), begin(subpts), end(subpts));
    }
    return partitions_memo[{n, k, m, M}] = pts;
}

template <typename I>
auto partition_sample_naive(I n, int k, I m, I M = std::numeric_limits<I>::max()) {
    vector<I> parts(k, m);
    int t = k - 1;
    n -= m * k;
    while (n > 0) {
        int i = intd(0, t)(mt);
        if (++parts[i] == M) {
            swap(parts[i], parts[t--]);
        }
        n--;
    }
    return parts;
}

long count_permutations(const vi& pt) {
    vi k;
    int i = 0, N = pt.size();
    while (i < N) {
        int j = i + 1;
        while (j < N && pt[i] == pt[j])
            j++;
        k.push_back(j - i), i = j;
    }
    return choose(N, k);
}

// *****

/**
 * Test normality of count distribution
 */
void verify_normality(vi& v) {
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
}

void stress_test_partition_sample_uniform(double F = 1'000) {
    const int N = 30, k = 5, m = 1, M = 10;
    long f = intfac(k);

    const auto& pts = generate_all_partitions(N, k, m, M);
    int P = pts.size(); // 98

    print("count_partitions({},{},1) = {}\n", N, k, count_partitions(N, k, m, M));
    print("# partitions: {}\n", pts.size());
    assert(count_partitions(N, k, m, M) == P);
    for (const auto& pt : pts)
        cout << pt << endl;

    unordered_map<vi, int> id;
    for (int i = 0; i < P; i++) {
        id[pts[i]] = i;
    }

    vi fast(P, 0), naive(P, 0);
    for (int t = 0, T = F * P; t < T; t++) {
        print_progress(t, T, "stress test partition sample uniform (fast)");
        auto pt = partition_sample(N, k, m, M);
        sort(begin(pt), end(pt));
        fast[id.at(pt)]++;
    }
    for (int t = 0, T = F * P; t < T; t++) {
        print_progress(t, T, "stress test partition sample uniform (naive)");
        auto pt = partition_sample_naive(N, k, m, M);
        sort(begin(pt), end(pt));
        naive[id.at(pt)]++;
    }

    clear_line();
    for (int i = 0; i < P; i++) {
        auto p = count_permutations(pts[i]);
        double fast_w = 1.0 * fast[i] / p, naive_w = 1.0 * naive[i] / p;
        print("{:12} -- {:5} | {:6.1f} {:6.1f}\n", pts[i], p, fast_w, naive_w);
        fast[i] *= f / p;
        naive[i] *= f / p;
    }

    verify_normality(fast);
    verify_normality(naive);
    print_ok("stress test partition sample uniform");
}

void stress_test_vec_sample(int T = 70000, int n = 4096, int k = 37) {
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
    stress_test_partition_sample_uniform();
    stress_test_vec_sample();
    scaling_test_int_sample();
    scaling_test_pair_sample();
    scaling_test_choose_sample();
    return 0;
}
