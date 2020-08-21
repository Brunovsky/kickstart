#include "../random.hpp"

using namespace std;

// *****

/**
 * Test normality of count distribution
 */
void normality_test(vector<int>& cnt) {
    int n = cnt.size();
    int m = *min_element(begin(cnt), end(cnt));
    int M = *max_element(begin(cnt), end(cnt));
    int sum = accumulate(begin(cnt), end(cnt), 0);
    double mean = 1.0 * sum / n;
    double variance = 0;
    for (int k : cnt) {
        variance += (k - mean) * (k - mean);
    }
    variance /= n;
    double rho = sqrt(variance);
    unordered_map<int, int> within; // count numbers within k standard deviations
    for (int k : cnt) {
        within[int(abs(k - mean) / rho)]++;
    }
    for (int i = 1; i <= 5; i++) {
        within[i] += within[i - 1];
    }

    printf("===== NORMALITY TEST =====\n");
    printf("sum: %d\n", sum);
    printf("min: %d  (%+.2lf)\n", m, (m - mean) / rho);
    printf("max: %d  (%+.2lf)\n", M, (M - mean) / rho);
    printf("mean: %.2lf\n", mean);
    printf("rho:  %.2lf\n", rho);
    printf("within 1 rho: %5.2lf%%\n", 100.0 * within[0] / n);
    printf("within 2 rho: %5.2lf%%\n", 100.0 * within[1] / n);
    printf("within 3 rho: %5.2lf%%\n", 100.0 * within[2] / n);
    printf("within 4 rho: %5.2lf%%\n", 100.0 * within[3] / n);
    printf("(remember, expected: 68.27 - 95.45 - 99.73)\n");
    assert(m > 0.5 * mean);
    assert(M < 2.0 * mean);
}

void test_sampler(int n = 53439, int k = 173, int runs = 30000) {
    int start = 87632;
    vector<int> univ(n);
    iota(begin(univ), end(univ), start);
    vector<int> cnt(n, 0);

    int i = 0;
    while (i++ < runs) {
        vector<int> sample = get_sample(univ, k);
        for (int m : sample) {
            assert(start <= m && m < start + n);
            cnt[m - start]++;
        }
        assert(sample.size() == uint(k));
        printf("\riteration %5d/%-5d", i, runs);
    }
    printf("\n");
    normality_test(cnt);
}

int main() {
    test_sampler();
    return 0;
}
