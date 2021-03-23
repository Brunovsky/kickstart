#ifndef RANDOM_HPP
#define RANDOM_HPP

#include "graph.hpp"
#include "hash.hpp"

// *****

default_random_engine mt(random_device{}());
using chard = uniform_int_distribution<char>;
using intd = uniform_int_distribution<int>;
using longd = uniform_int_distribution<long>;
using ulongd = uniform_int_distribution<size_t>;
using reald = uniform_real_distribution<double>;
using binomd = binomial_distribution<long>;
using boold = bernoulli_distribution;

using int_sample_t = vector<int>;
using pair_sample_t = vector<edge_t>;
using partition_t = vector<int>;

int different(int u, int v1, int v2) {
    assert(v1 <= v2 && (v1 != u || v2 != u));
    if (v1 == v2)
        return v1;
    intd dist(v1, v2 - 1);
    int n = dist(mt);
    return n + (n >= u);
}

/**
 * Run k iterations of Fisher-Yates shuffle over a vector,
 * shuffling its first k elements. Complexity linear in k and E[mt] = k.
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
 * Generate a sorted sample of k distinct integers from the range [a..b]
 * It must hold that a <= b and k <= n = b - a + 1.
 * Complexity: O(k) with E[mt] <= 3k.
 */
int_sample_t int_sample(int k, int a, int b, bool complement = false) {
    int ab = b - a + 1;
    assert(a <= b && 0 <= k && k <= ab);
    if (3 * k >= 2 * ab) {
        return int_sample(ab - k, a, b, !complement);
    }

    intd dist(a, b);
    unordered_set<int> seen;
    seen.reserve(k);
    while (k--) {
        int n;
        do {
            n = dist(mt);
        } while (seen.count(n));
        seen.insert(n);
    }

    int_sample_t sample;
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
 * Generate a sorted sample of k integer pairs (x,y) where x, y are taken from the range
 * [a..b] and x < y.
 * It must hold that a <= b and k <= (n choose 2) where n = b - a + 1.
 * Complexity: O(k) with E[mt] <= 6k.
 */
pair_sample_t choose_sample(int k, int a, int b, bool complement = false) {
    long ab = 1L * (b - a + 1) * (b - a + 2) / 2 - (b - a + 1);
    assert(a <= b && 0 <= k && k <= ab);
    if (3 * k >= 2 * ab) {
        return choose_sample(ab - k, a, b, !complement);
    }

    intd dist(a, b);
    unordered_set<pair<int, int>, pair_hasher> seen;
    seen.reserve(k);
    while (k--) {
        pair<int, int> p;
        do {
            p = minmax(dist(mt), dist(mt));
        } while (p.first == p.second || seen.count(p));
        seen.insert(p);
    }

    pair_sample_t sample;
    if (complement) {
        for (int x = a; x <= b; x++)
            for (int y = x + 1; y <= b; y++)
                if (!seen.count({x, y}))
                    sample.push_back({x, y});
    } else if (3 * k >= b - a + 1) {
        for (int x = a; x <= b; x++)
            for (int y = x + 1; y <= b; y++)
                if (seen.count({x, y}))
                    sample.push_back({x, y});
    } else {
        for (auto& p : seen)
            sample.push_back({p.first, p.second});
        sort(begin(sample), end(sample));
    }
    return sample;
}

/**
 * Generate an unsorted sample of k integer pairs (x,y) where x is taken from the range
 * [a..b] and y is taken from the range [c..d].
 * It must hold that a <= b, c <= d, and k <= n x m = (b - a + 1)(d - c + 1).
 * Complexity: O(k) with E[mt] = 6k.
 */
pair_sample_t pair_sample(int k, int a, int b, int c, int d, bool complement = false) {
    long ab = b - a + 1, cd = d - c + 1;
    assert(a <= b && c <= d && 0 <= k && k <= ab * cd);
    if (3 * k >= 2 * ab * cd) {
        return pair_sample(ab * cd - k, a, b, c, d, !complement);
    }

    intd dist0(a, b), dist1(c, d);
    unordered_set<array<int, 2>, vec_hasher> seen;
    seen.reserve(k);
    while (k--) {
        int x, y;
        do {
            x = dist0(mt), y = dist1(mt);
        } while (seen.count({x, y}));
        seen.insert({x, y});
    }

    pair_sample_t sample;
    if (complement) {
        for (int x = a; x <= b; x++)
            for (int y = c; y <= d; y++)
                if (!seen.count({x, y}))
                    sample.push_back({x, y});
    } else if (3 * k >= b - a + 1) {
        for (int x = a; x <= b; x++)
            for (int y = c; y <= d; y++)
                if (seen.count({x, y}))
                    sample.push_back({x, y});
    } else {
        copy(begin(seen), end(seen), back_inserter(sample));
        sort(begin(sample), end(sample));
    }
    return sample;
}

/**
 * Run integer selection sampling over a vector, sampling k elements.
 * Complexity: O(k) and E[mt] = 3k.
 */
template <typename T>
vector<T> vec_sample(const vector<T>& univ, int k) {
    int n = univ.size();
    assert(0 < k && k <= n);
    vector<int> idx = int_sample(k, 0, n - 1);
    vector<int> sample;
    for (int i = 0; i < k; i++)
        sample.push_back(univ[idx[i]]);
    return sample;
}

/**
 * Generate an array of size n where each element parent[i] is selected
 * uniformly at random from [0..i-1] and parent[0] = 0.
 * Complexity: O(n)
 */
parent_t parent_sample(int n) {
    parent_t parent(n);
    for (int i = 1; i < n; i++) {
        intd dist(0, i - 1);
        parent[i] = dist(mt);
    }
    return parent;
}

/**
 * Generate a partition of n into k parts each of size at least m.
 * It must hold that k > 0.
 * Complexity: faster than linear
 */
template <typename I = int>
auto partition_sample(I n, int k, I m = 1) {
    vector<I> parts(k, m);
    intd dist(0, k - 1);
    n -= m * k;
    assert(n >= 0);
    while (n > 0) {
        I add = (n + k - 1) / k;
        parts[dist(mt)] += add;
        n -= add;
    }
    return parts;
}

/**
 * Like partition_sample but the first and last levels have size exactly 1.
 */
template <typename I = int>
auto partition_sample_flow(I V, I ranks, I m = 1) {
    auto R = partition_sample(V - 2, ranks - 2, m);
    R.insert(R.begin(), 1);
    R.insert(R.end(), 1);
    return R;
}

/**
 * Generate a supply partition with n elements whose total sum is c and minimum is m.
 */
template <typename I = int>
auto supply_sample(int n, int positives, int negatives, I sum, I m = 1) {
    assert(n && positives && negatives && sum && positives + negatives <= n);
    vector<I> vec(n, 0);
    auto pos = partition_sample(sum, positives, m);
    auto neg = partition_sample(sum, negatives, m);
    auto idx = int_sample(positives + negatives, 0, n - 1);
    shuffle(begin(idx), end(idx), mt);
    for (int i = 0; i < positives; i++)
        vec[idx[i]] = pos[i];
    for (int i = 0; i < negatives; i++)
        vec[idx[i + positives]] = neg[i];
    return vec;
}

/**
 * Vector of size n with elements taken uniformly from [a,b], possibly repeated
 */
template <typename T = int>
auto int_gen(int n, T a, T b) {
    vector<T> vec(n);
    uniform_int_distribution<T> dist(a, b);
    for (int i = 0; i < n; i++)
        vec[i] = dist(mt);
    return vec;
}
template <typename T = int>
auto real_gen(int n, T a, T b) {
    vector<T> vec(n);
    uniform_real_distribution<T> dist(a, b);
    for (int i = 0; i < n; i++)
        vec[i] = dist(mt);
    return vec;
}

#endif // RANDOM_HPP
