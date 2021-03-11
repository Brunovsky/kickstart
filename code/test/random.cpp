#include "../random.hpp"

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

void stress_test_vec_sample(int n = 93439, int k = 173, int runs = 30000) {
    int start = 87632;
    vector<int> univ(n);
    iota(begin(univ), end(univ), start);
    shuffle(begin(univ), end(univ), mt);
    vector<int> cnt(n, 0);

    for (int i = 0; i < runs; i++) {
        print_progress(i, runs, "stress test vec_sample");
        vector<int> sample = vec_sample(univ, k);
        for (int m : sample) {
            assert(start <= m && m < start + n);
            cnt[m - start]++;
        }
        assert(sample.size() == k);
    }
    print("\n");
    verify_normality(cnt);
}

int main() {
    stress_test_vec_sample();
    return 0;
}
