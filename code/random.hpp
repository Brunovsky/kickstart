#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

default_random_engine mt(random_device{}());
using chard = uniform_int_distribution<char>;
using intd = uniform_int_distribution<int>;
using longd = uniform_int_distribution<long>;
using ulongd = uniform_int_distribution<size_t>;
using reald = uniform_real_distribution<double>;
using binomd = binomial_distribution<long>;
using boold = bernoulli_distribution;

using edge_t = array<int, 2>;
using edges_t = vector<array<int, 2>>;

// *****

int different(int u, int v1, int v2) {
    assert(v1 <= v2 && (v1 != u || v2 != u));
    if (v1 == v2)
        return v1;
    intd dist(v1, v2 - 1);
    int v = dist(mt);
    return v + (v >= u);
}

auto different(int v1, int v2) {
    assert(v1 < v2);
    int u = intd(v1, v2)(mt);
    int v = intd(v1, v2 - 1)(mt);
    return array<int, 2>{u, v + (v >= u)};
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
 * Generate a sorted sample of k distinct integers from the range [a..b)
 * It must hold that a <= b and k <= n = b - a.
 * Complexity: O(k log k) with E[mt] <= 3k.
 */
vector<int> int_sample(long k, int a, int b, bool complement = false) {
    if ((k == 0 && !complement) || a >= b)
        return {};

    long ab = b - a, remaining = k;
    assert(ab >= 0 && 0 <= k && k <= ab);
    if (3 * k >= 2 * ab) {
        return int_sample(ab - k, a, b, !complement);
    }

    intd dist(a, b - 1);
    unordered_set<int> seen;
    seen.reserve(k);
    while (remaining--) {
        int n;
        do {
            n = dist(mt);
        } while (seen.count(n));
        seen.insert(n);
    }

    vector<int> sample;
    sample.reserve(seen.size());
    if (complement) {
        for (int n = a; n < b; n++)
            if (!seen.count(n))
                sample.push_back(n);
    } else if (3 * k >= ab) {
        for (int n = a; n < b; n++)
            if (seen.count(n))
                sample.push_back(n);
    } else {
        copy(begin(seen), end(seen), back_inserter(sample));
        sort(begin(sample), end(sample));
    }
    return sample;
}

auto int_sample_p(double p, int a, int b) {
    long ab = b - a;
    return int_sample(binomd(ab, min(p, 1.0))(mt), a, b);
}

/**
 * Generate a sorted sample of k integer pairs (x,y) where x, y are taken from the range
 * [a..b) and x < y.
 * It must hold that a <= b and k <= (n choose 2) where n = b - a.
 * Complexity: O(k log k) with E[mt] <= 6k.
 */
edges_t choose_sample(long k, int a, int b, bool complement = false) {
    if ((k == 0 && !complement) || a >= b - 1)
        return {};

    static_assert(sizeof(int) == 4);
    long ab = 1L * (b - a) * (b - a - 1) / 2, remaining = k;
    assert(ab >= 0 && 0 <= k && k <= ab);
    if (3 * k >= 2 * ab) {
        return choose_sample(ab - k, a, b, !complement);
    }

    intd distx(a, b - 1), disty(a, b - 2);
    unordered_set<long> seen;
    seen.reserve(k);
    while (remaining--) {
        int x, y;
        do {
            x = distx(mt), y = disty(mt), tie(x, y) = minmax(x, y + (y >= x));
        } while (seen.count(long(x) << 32 | y));
        seen.insert(long(x) << 32 | y);
    }

    edges_t sample;
    sample.reserve(seen.size());
    if (complement) {
        for (int x = a; x < b; x++)
            for (int y = x + 1; y < b; y++)
                if (!seen.count(long(x) << 32 | y))
                    sample.push_back({x, y});
    } else if (12 * k >= ab) {
        for (int x = a; x < b; x++)
            for (int y = x + 1; y < b; y++)
                if (seen.count(long(x) << 32 | y))
                    sample.push_back({x, y});
    } else {
        int x, y;
        for (long n : seen)
            x = n >> 32, y = n & 0xffffffffL, sample.push_back({x, y});
        sort(begin(sample), end(sample));
    }
    return sample;
}

auto choose_sample_p(double p, int a, int b) {
    long ab = 1L * (b - a) * (b - a - 1) / 2;
    return choose_sample(binomd(ab, min(p, 1.0))(mt), a, b);
}

/**
 * Generate a sorted sample of k integer pairs (x,y) where x is taken from the range
 * [a..b) and y is taken from the range [c..d).
 * It must hold that a <= b, c <= d, and k <= nm = (b - a)(d - c).
 * Complexity: O(k log k) with E[mt] <= 6k.
 */
edges_t pair_sample(long k, int a, int b, int c, int d, bool complement = false) {
    if ((k == 0 && !complement) || a >= b || c >= d)
        return {};

    static_assert(sizeof(int) == 4);
    long ab = b - a, cd = d - c, remaining = k;
    assert(ab >= 0 && cd >= 0 && 0 <= k && k <= ab * cd);
    if (3 * k >= 2 * ab * cd) {
        return pair_sample(ab * cd - k, a, b, c, d, !complement);
    }

    intd distx(a, b - 1), disty(c, d - 1);
    unordered_set<long> seen;
    seen.reserve(k);
    while (remaining--) {
        int x, y;
        do {
            x = distx(mt), y = disty(mt);
        } while (seen.count(long(x) << 32 | y));
        seen.insert(long(x) << 32 | y);
    }

    edges_t sample;
    sample.reserve(seen.size());
    if (complement) {
        for (int x = a; x < b; x++)
            for (int y = c; y < d; y++)
                if (!seen.count(long(x) << 32 | y))
                    sample.push_back({x, y});
    } else if (12 * k >= ab * cd) {
        for (int x = a; x < b; x++)
            for (int y = c; y < d; y++)
                if (seen.count(long(x) << 32 | y))
                    sample.push_back({x, y});
    } else {
        int x, y;
        for (long n : seen)
            x = n >> 32, y = n & 0xffffffffL, sample.push_back({x, y});
        sort(begin(sample), end(sample));
    }
    return sample;
}

auto pair_sample_p(double p, int a, int b, int c, int d) {
    long ab = b - a, cd = d - c;
    return pair_sample(binomd(ab * cd, min(p, 1.0))(mt), a, b, c, d);
}

/**
 * Generate an unsorted sample of k integers pairs (x,y) where x and y are taken from the
 * range [a..b) and x != y.
 * It must hold that a <= b, and k <= n(n - 1) where n = b - a.
 * Complexity: O(k log k) with E[mt] <= 6k.
 */
edges_t distinct_pair_sample(long k, int a, int b, bool complement = false) {
    auto g = pair_sample(k, a, b, a, b - 1, complement);
    for (auto& [u, v] : g)
        v += v >= u;
    return g;
}

auto distinct_pair_sample_p(double p, int a, int b) {
    long ab = 1L * (b - a) * (b - a - 1);
    return distinct_pair_sample(binomd(ab, min(p, 1.0))(mt), a, b);
}

/**
 * Run integer selection sampling over a vector, sampling k elements.
 * Complexity: O(k) and E[mt] = 3k.
 */
template <typename T>
auto vec_sample(const vector<T>& univ, int k) {
    int n = univ.size();
    assert(0 <= k && k <= n);
    vector<int> idx = int_sample(k, 0, n);
    vector<T> sample(k);
    for (int i = 0; i < k; i++)
        sample[i] = univ[idx[i]];
    return sample;
}

template <size_t k, typename T>
auto array_sample(const vector<T>& univ) {
    int n = univ.size();
    assert(0 < k && k <= n);
    vector<int> idx = int_sample(k, 0, n);
    array<T, k> sample;
    for (int i = 0; i < k; i++)
        sample[i] = univ[idx[i]];
    return sample;
}

/**
 * Generate an array of size n where each element parent[i] is selected
 * uniformly at random from [0..i-1] and parent[0] = 0.
 * Complexity: O(n)
 */
auto parent_sample(int n) {
    vector<int> parent(n);
    for (int i = 1; i < n; i++) {
        intd dist(0, i - 1);
        parent[i] = dist(mt);
    }
    return parent;
}

/**
 * Generate a random partition of n into k parts each of size between m and M.
 * It must hold that k > 0 and 0 <= m <= M and mk <= n <= Mk.
 * Complexity: faster than linear
 * Not uniform, but close if M is sufficiently restrictive
 */
template <typename I = int>
auto partition_sample(I n, int k, I m = 1, I M = std::numeric_limits<I>::max()) {
    assert(n >= 0 && k > 0 && m >= 0 && m <= n / k && (n + k - 1) / k <= M);

    vector<I> parts(k, m);
    n -= m * k--;
    while (n > 0) {
        I add = (n + k) / (k + 1);
        int i = intd(0, k)(mt);
        if (parts[i] + add >= M) {
            n -= M - parts[i];
            parts[i] = M;
            swap(parts[i], parts[k--]);
        } else {
            n -= add;
            parts[i] += add;
        }
    }
    shuffle(begin(parts), end(parts), mt);
    return parts;
}

/**
 * Generate a partition of n into k parts each of size between m_i and M_i.
 * It must hold that k > 0 and 0 <= m_i <= M_i and SUM(m_i) <= n <= SUM(M_i)
 * Complexity: faster than linear
 * Not uniform, but close if M is sufficiently restrictive
 */
template <typename I = int>
auto partition_sample(I n, int k, const vector<I>& m, const vector<I>& M) {
    assert(k > 0 && k <= int(m.size()) && k <= int(M.size()));
    assert(accumulate(begin(m), end(m), I(0)) <= n);
    assert(n <= accumulate(begin(M), end(M), I(0)));

    vector<I> parts = m;
    vector<int> id(k);
    parts.resize(k--);
    iota(begin(id), end(id), 0);
    n -= accumulate(begin(m), end(m), I(0));
    while (n > 0) {
        I add = (n + k) / (k + 1);
        int i = intd(0, k)(mt), j = id[i];
        if (parts[j] + add >= M[j]) {
            n -= M[j] - parts[j];
            parts[j] = M[j];
            swap(id[i], id[k--]);
        } else {
            n -= add;
            parts[j] += add;
        }
    }
    return parts;
}

/**
 * Like partition_sample but the first and last levels have size exactly 1.
 */
template <typename I = int>
auto partition_sample_flow(I V, I ranks, I m = 1, I M = std::numeric_limits<I>::max()) {
    auto R = partition_sample(V - 2, ranks - 2, m, M);
    R.insert(R.begin(), 1);
    R.insert(R.end(), 1);
    return R;
}
template <typename I = int>
auto partition_sample_flow(I V, I ranks, const vector<I>& m, const vector<I>& M) {
    auto R = partition_sample(V - 2, ranks - 2, m, M);
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

/**
 * any string, with length m with characters [a,b]
 */
auto generate_any_string(int m, char a = 'a', char b = 'z') {
    chard distchar(a, b);
    string str(m, 0);
    for (int j = 0; j < m; j++)
        str[j] = distchar(mt);
    return str;
}

/**
 * n random strings, with length [minlen,maxlen] with characters [a,b]
 */
auto generate_any_strings(int n, int minlen, int maxlen, char a = 'a', char b = 'z') {
    intd distm(minlen, maxlen);
    vector<string> strs(n);
    for (int i = 0; i < n; i++) {
        strs[i] = generate_any_string(distm(mt), a, b);
    }
    return strs;
}

#endif // RANDOM_HPP
