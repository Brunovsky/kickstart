#include "test_utils.hpp"
#include "../hash.hpp"
#include "../numeric/math.hpp"
#include "../linear/matrix.hpp"
#include "../numeric/partitions.hpp"

inline namespace detail {

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

} // namespace detail

/**
 * Test normality of count distribution
 */
void verify_normality(vi& v, bool show_histogram = false) {
    int n = v.size();
    int mi = min_element(begin(v), end(v)) - begin(v);
    int Mi = max_element(begin(v), end(v)) - begin(v);
    int m = v[mi], M = v[Mi];
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

    printcl("===== NORMALITY TEST =====\n");
    if (show_histogram) {
        long each = sum / n;
        for (int i = 0; i < n; i++) {
            print("{:2} -- {}\n", i, string(v[i] * 40 / each, '*'));
        }
    }
    print("sum: {}\n", sum);
    print("min: {}  ({:+.2f})  (i={})\n", m, (m - mean) / rho, mi);
    print("max: {}  ({:+.2f})  (i={})\n", M, (M - mean) / rho, Mi);
    print("mean: {:.2f}\n", mean);
    print("rho:  {:.2f}\n", rho);
    print("within 1 rho: {:5.2f}%\n", 100.0 * within[0] / n);
    print("within 2 rho: {:5.2f}%\n", 100.0 * within[1] / n);
    print("within 3 rho: {:5.2f}%\n", 100.0 * within[2] / n);
    print("within 4 rho: {:5.2f}%\n", 100.0 * within[3] / n);
    print("(remember, expected: 68.27 - 95.45 - 99.73)\n");
}

void stress_test_partition_sample_uniform(double F = 100) {
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

    for (int i = 0; i < P; i++) {
        auto p = count_permutations(pts[i]);
        double fast_w = 1.0 * fast[i] / p, naive_w = 1.0 * naive[i] / p;
        printcl("{:12} -- {:5} | {:6.1f} {:6.1f}\n", pts[i], p, fast_w, naive_w);
        fast[i] *= f / p;
        naive[i] *= f / p;
    }

    verify_normality(fast);
    verify_normality(naive);
}

void stress_test_vec_sample(int n = 4096, int k = 37) {
    int start = 87632;
    vector<int> univ(n);
    iota(begin(univ), end(univ), start);
    shuffle(begin(univ), end(univ), mt);
    vector<int> cnt(n, 0);

    LOOP_FOR_DURATION_TRACKED (4s, now) {
        print_time(now, 4s, "stress test vec_sample");

        vector<int> sample = vec_sample(univ, k);
        for (int m : sample) {
            assert(start <= m && m < start + n);
            cnt[m - start]++;
        }
        assert(int(sample.size()) == k);
    }

    verify_normality(cnt);
}

void stress_test_int_sample(int n = 1000) {
    vector<int> cnt(n, 0);
    intd distk(0, n - 1);

    LOOP_FOR_DURATION_TRACKED (4s, now) {
        print_time(now, 4s, "stress test int sample");

        int k = distk(mt);
        auto nums = int_sample(k, 0, n);
        assert(is_sorted(begin(nums), end(nums)));
        assert(int(nums.size()) == k);
        for (int i = 1; i < k; i++) {
            assert(nums[i - 1] < nums[i]);
        }
        for (int i : nums) {
            assert(0 <= i && i < n);
            cnt[i]++;
        }
    }

    verify_normality(cnt);
}

void stress_test_choose_sample(int n = 45) {
    vector<vector<int>> matcnt(n, vector<int>(n, 0));
    intd distk(0, n * (n - 1) / 2);

    LOOP_FOR_DURATION_TRACKED (4s, now) {
        print_time(now, 4s, "stress test choose sample");

        int k = distk(mt);
        auto nums = choose_sample(k, 0, n);
        assert(is_sorted(begin(nums), end(nums)));
        assert(int(nums.size()) == k);
        for (int i = 1; i < k; i++) {
            assert(nums[i - 1] < nums[i]);
        }
        for (auto [x, y] : nums) {
            assert(0 <= x && x < n);
            assert(0 <= y && y < n);
            assert(x < y);
            matcnt[x][y]++;
        }
    }

    vector<int> cnt;
    for (int x = 0; x < n; x++) {
        for (int y = x + 1; y < n; y++) {
            cnt.push_back(matcnt[x][y]);
        }
    }

    verify_normality(cnt);
}

void stress_test_pair_sample(int n = 31, int m = 31) {
    vector<vector<int>> matcnt(n, vector<int>(m, 0));
    intd distk(0, n * m);

    LOOP_FOR_DURATION_TRACKED (4s, now) {
        print_time(now, 4s, "stress test pair sample");

        int k = distk(mt);
        auto nums = pair_sample(k, 0, n, 0, m);
        assert(is_sorted(begin(nums), end(nums)));

        assert(int(nums.size()) == k);
        for (int i = 1; i < k; i++) {
            assert(nums[i - 1] < nums[i]);
        }
        for (auto [x, y] : nums) {
            assert(0 <= x && x < n);
            assert(0 <= y && y < m);
            matcnt[x][y]++;
        }
    }

    vector<int> cnt;
    for (int x = 0; x < n; x++) {
        for (int y = 0; y < m; y++) {
            cnt.push_back(matcnt[x][y]);
        }
    }

    verify_normality(cnt);
}

void scaling_test_int_sample() {
    constexpr long MAX_SAMPLES = 50'000'000;
    static const vector<int> abs = {
        10, 100, 1000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000,
    };
    static const vector<int> sparse = {
        2,     4,     9,     20,     40,     90,     200,     400,     900,
        2'000, 4'000, 9'000, 20'000, 40'000, 90'000, 200'000, 400'000, 900'000,
    };
    static const vector<double> dense = {
        .01, .025, .05, .1, .2, .3, .5, .75, .9, .98,
    };
    const auto runtime = 20000ms / (abs.size() * (sparse.size() + dense.size()));

    intd dista(0, 25000);
    map<pair<int, int>, string> table_abs;
    map<pair<int, double>, string> table_dense;

    for (int k : sparse) {
        for (int n : abs) {
            printcl(" int sample sparse test x{} n={}", k, n);
            if (k > n)
                continue;

            START(sampler);
            LOOP_FOR_DURATION_TRACKED_RUNS (runtime, now, runs) {
                int a = dista(mt), b = a + n;
                auto res = int_sample(k, a, b);
                assert(int(res.size()) == k);
            }
            TIME(sampler);

            table_abs[{n, k}] = FORMAT_EACH(sampler, 1.0 * k * runs);
        }
    }

    for (double p : dense) {
        for (int n : abs) {
            printcl(" int sample dense test p={} n={}", p, n);
            if (p * n > MAX_SAMPLES)
                continue;

            START(sampler);
            LOOP_FOR_DURATION_TRACKED_RUNS (runtime, now, runs) {
                int a = dista(mt), b = a + n;
                int_sample_p(p, a, b);
            }
            TIME(sampler);

            table_dense[{n, p}] = FORMAT_EACH(sampler, p * n * runs);
        }
    }

    print_time_table(table_abs, "int_sample abs");
    print_time_table(table_dense, "int_sample dense");
}

void scaling_test_choose_sample() {
    constexpr long MAX_SAMPLES = 50'000'000;
    static const vector<int> abs_sparse = {
        100, 1000, 10'000, 100'000, 1'000'000,
    };
    static const vector<int> sparse = {
        10, 40, 90, 400, 900, 4'000, 9'000, 40'000, 90'000, 400'000, 900'000,
    };
    static const vector<int> abs_dense = {
        10, 40, 100, 300, 800, 2'000, 5'000,
    };
    static const vector<double> dense = {
        .01, .025, .05, .1, .2, .3, .5, .75, .9, .98,
    };
    const auto runtime = 20000ms / (abs_sparse.size() * sparse.size() +
                                    abs_dense.size() * dense.size());

    intd dista(0, 25000);
    map<pair<int, int>, string> table_abs;
    map<pair<int, double>, string> table_dense;

    for (int k : sparse) {
        for (int n : abs_sparse) {
            printcl(" choose sample sparse test x{} n={}", k, n);
            if (k > choose(n, 2))
                continue;

            START(sampler);
            LOOP_FOR_DURATION_TRACKED_RUNS (runtime, now, runs) {
                int a = dista(mt), b = a + n;
                auto res = choose_sample(k, a, b);
                assert(int(res.size()) == k);
            }
            TIME(sampler);

            table_abs[{n, k}] = FORMAT_EACH(sampler, 1.0 * k * runs);
        }
    }

    for (double p : dense) {
        for (int n : abs_dense) {
            printcl(" choose sample dense test p={} n={}", p, n);
            if (p * choose(n, 2) > MAX_SAMPLES)
                continue;

            START(sampler);
            LOOP_FOR_DURATION_TRACKED_RUNS (runtime, now, runs) {
                int a = dista(mt), b = a + n;
                choose_sample_p(p, a, b);
            }
            TIME(sampler);

            table_dense[{n, p}] = FORMAT_EACH(sampler, p * choose(n, 2) * runs);
        }
    }

    print_time_table(table_abs, "choose_sample abs");
    print_time_table(table_dense, "choose_sample dense");
}

void scaling_test_pair_sample() {
    constexpr long MAX_SAMPLES = 50'000'000;
    static const vector<array<int, 2>> abs_sparse = {
        {100, 100}, {100, 500},  {300, 300},  {100, 3000},
        {500, 500}, {500, 1000}, {300, 2000}, {1000, 1000},
    };
    static const vector<int> sparse = {
        10, 40, 90, 400, 900, 4'000, 9'000, 40'000, 90'000, 400'000, 900'000,
    };
    static const vector<array<int, 2>> abs_dense = {
        {20, 20},    {50, 50},     {100, 100},   {500, 500},
        {100, 1000}, {1000, 1000}, {3000, 3000},
    };
    static const vector<double> dense = {
        .01, .025, .05, .1, .2, .3, .5, .75, .9, .98,
    };
    const auto runtime = 20000ms / (abs_sparse.size() * sparse.size() +
                                    abs_dense.size() * dense.size());

    intd dista(0, 25000), distc(30000, 35000);
    map<pair<pair<int, int>, int>, string> table_abs;
    map<pair<pair<int, int>, double>, string> table_dense;

    for (int k : sparse) {
        for (auto [n, m] : abs_sparse) {
            printcl(" pair sample sparse test x{} n,m={},{}", k, n, m);
            if (k > n * m)
                continue;

            START(sampler);
            LOOP_FOR_DURATION_TRACKED_RUNS (runtime, now, runs) {
                int a = dista(mt), b = a + n, c = distc(mt), d = c + m;
                auto res = pair_sample(k, a, b, c, d);
                assert(int(res.size()) == k);
            }
            TIME(sampler);

            table_abs[{{n, m}, k}] = FORMAT_EACH(sampler, 1.0 * k * runs);
        }
    }

    for (double p : dense) {
        for (auto [n, m] : abs_dense) {
            printcl(" pair sample dense test p={} n,m={},{}", p, n, m);
            if (p * n * m > MAX_SAMPLES)
                continue;

            START(sampler);
            LOOP_FOR_DURATION_TRACKED_RUNS (runtime, now, runs) {
                int a = dista(mt), b = a + n, c = distc(mt), d = c + m;
                pair_sample_p(p, a, b, c, d);
            }
            TIME(sampler);

            table_dense[{{n, m}, p}] = FORMAT_EACH(sampler, p * n * m * runs);
        }
    }

    print_time_table(table_abs, "pair_sample abs");
    print_time_table(table_dense, "pair_sample dense");
}

int main() {
    RUN_BLOCK(stress_test_partition_sample_uniform());
    RUN_BLOCK(stress_test_vec_sample());
    RUN_BLOCK(stress_test_int_sample());
    RUN_BLOCK(stress_test_choose_sample());
    RUN_BLOCK(stress_test_pair_sample());
    RUN_BLOCK(scaling_test_int_sample());
    RUN_BLOCK(scaling_test_choose_sample());
    RUN_BLOCK(scaling_test_pair_sample());
    return 0;
}
