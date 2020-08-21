#include <bits/stdc++.h>

using namespace std;

// *****

default_random_engine mt(random_device{}());
using intd = uniform_int_distribution<int>;
using longd = uniform_int_distribution<long>;
using ulongd = uniform_int_distribution<size_t>;
using reald = uniform_real_distribution<double>;
using binomd = binomial_distribution<int>;
using boold = bernoulli_distribution;

/**
 * Generate a sorted sample of k distinct integers from the range [a..b]
 * It must hold that a <= b and k <= n = b - a + 1.
 * Expected worst-case complexity is 3k random calls, best case is k random calls.
 */
vector<int> get_sample(int k, int a, int b, bool complement = false) {
    assert(a <= b && k <= b - a + 1);
    // k >= 0.66n where n = b - a + 1.
    if (3 * k >= 2 * (b - a + 1)) {
        return get_sample(b - a + 1 - k, a, b, !complement);
    }

    intd dist(a, b);
    unordered_set<int> seen;
    while (k--) {
        int n;
        do {
            n = dist(mt);
        } while (seen.count(n));
        seen.insert(n);
    }
    vector<int> sample;
    if (complement) {
        for (int n = a; n <= b; n++)
            if (!seen.count(n))
                sample.push_back(n);
    } else if (3 * k >= b - a + 1) {
        for (int n = a; n <= b; n++)
            if (seen.count(n))
                sample.push_back(n);
    } else {
        copy(begin(seen), end(seen), back_inserter(sample));
        sort(begin(sample), end(sample));
    }
    return sample;
}

/**
 * Run k iterations of Fisher-Yates shuffle over a vector,
 * shuffling its first k elements. Complexity linear in k.
 */
template <typename T>
void fisher_yates(vector<T>& univ, int k = -1) {
    int n = univ.size(), i = 0;
    k = k < 0 ? n : k;
    assert(k <= n);
    while (k-- && i < n - 2) {
        intd dist(i, n - 1);
        int j = dist(mt);
        swap(univ[i], univ[j]);
        i++;
    }
}

/**
 * Run integer selection sampling over a vector,
 * sampling k elements. Expected complexity is k.
 */
template <typename T>
vector<T> get_sample(const vector<T>& univ, int k) {
    int n = univ.size();
    assert(0 < k && k <= n);
    vector<int> idx = get_sample(k, 0, n - 1);
    vector<int> sample;
    for (int i = 0; i < k; i++)
        sample.push_back(univ[idx[i]]);
    return sample;
}
