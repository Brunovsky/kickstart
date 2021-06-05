#ifndef RANDOM_HPP
#define RANDOM_HPP

#include "hash.hpp"
#include "algo/sort.hpp"

// *****

default_random_engine mt(random_device{}());
using chard = uniform_int_distribution<char>;
using intd = uniform_int_distribution<int>;
using longd = uniform_int_distribution<long>;
using ulongd = uniform_int_distribution<size_t>;
using reald = uniform_real_distribution<double>;
using binomd = binomial_distribution<long>;
using geod = geometric_distribution<int>;
using intnd = normal_distribution<int>;
using longnd = normal_distribution<long>;
using boold = bernoulli_distribution;

using edge_t = array<int, 2>;
using edges_t = vector<array<int, 2>>;

// *****

int different(int u, int v1, int v2) {
    assert(v1 < v2 && (v1 != u || v1 + 1 < v2));
    if (v1 + 1 == v2)
        return v1;
    intd dist(v1, v2 - 2);
    int v = dist(mt);
    return v + (v >= u);
}

auto different(int v1, int v2) {
    assert(v1 + 1 < v2);
    int u = intd(v1, v2 - 1)(mt);
    int v = intd(v1, v2 - 2)(mt);
    tie(u, v) = minmax(u + 0, v + (v >= u));
    return array<int, 2>{u, v};
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

template <typename T>
void reservoir_sample_inplace(vector<T>& univ, int k) {
    int N = univ.size();
    assert(k <= N);
    for (int i = k; i < N; i++) {
        intd dist(0, i);
        int j = dist(mt);
        if (j < k) {
            univ[j] = univ[i];
        }
    }
    univ.resize(k);
}

/**
 * Generate a sorted sample of k distinct integers from the range [a..b)
 * It must hold that a <= b and k <= n = b - a.
 * Complexity: O(k log k) with E[mt] <= 3k.
 */
template <typename T = int, typename I = int>
vector<T> int_sample(int k, I a, I b) {
    assert(k <= 100'000'000); // don't try anything crazy
    if (k == 0 || a >= b)
        return {};

    using sample_t = vector<T>;
    long univ = b - a;
    assert(univ >= 0 && 0 <= k && k <= univ);

    // 1/5 One sample -- sample any
    if (k == 1) {
        uniform_int_distribution<I> distn(a, b - 1);
        sample_t sample = {distn(mt)};
        return sample;
    }

    // 2/5 Full sample -- run iota
    if (k == univ) {
        sample_t whole(univ);
        iota(begin(whole), end(whole), a);
        return whole;
    }

    // 3/5 Majority sample: run negative bitset sampling
    if (k >= univ / 2) {
        vector<bool> unsampled(univ, false);
        int included = univ;
        intd disti(0, univ - 1);
        while (included > k) {
            int i = disti(mt);
            included -= !unsampled[i];
            unsampled[i] = true;
        }
        sample_t sample(k);
        for (int i = 0, n = 0; i < k; n++) {
            if (!unsampled[n]) {
                sample[i++] = a + n;
            }
        }
        return sample;
    }

    // 4/5 Large minority sample: run positive bitset sampling
    if (k >= univ / 12) {
        vector<bool> sampled(univ, false);
        int included = 0;
        intd disti(0, univ - 1);
        while (included < k) {
            int i = disti(mt);
            included += !sampled[i];
            sampled[i] = true;
        }
        sample_t sample(k);
        for (int i = 0, n = 0; i < k; n++) {
            if (sampled[n]) {
                sample[i++] = a + n;
            }
        }
        return sample;
    }

    // 5/5 Large sample: run repeated sampling
    sample_t sample(k);
    uniform_int_distribution<I> dist(a, b - 1);
    for (int i = 0; i < k; i++) {
        sample[i] = dist(mt);
    }
    sort(begin(sample), end(sample));
    int S = unique(begin(sample), end(sample)) - begin(sample);
    while (S < k) {
        int M = S;
        do {
            for (int i = M; i < k; i++) {
                sample[i] = dist(mt);
            }
            sort(begin(sample) + M, end(sample));
            inplace_merge(begin(sample) + S, begin(sample) + M, end(sample));
            M = unique(begin(sample) + S, end(sample)) - begin(sample);
        } while (M < k);

        inplace_merge(begin(sample), begin(sample) + S, end(sample));
        S = unique(begin(sample), end(sample)) - begin(sample);
    }
    return sample;
}

template <typename T = int, typename I = int>
auto int_sample_p(double p, I a, I b) {
    long ab = b - a;
    return int_sample<T>(binomd(ab, min(p, 1.0))(mt), a, b);
}

/**
 * Generate a sorted sample of k integer pairs (x,y) where x, y are taken from the range
 * [a..b) and x < y.
 * It must hold that a <= b and k <= (n choose 2) where n = b - a.
 * Complexity: O(k log k) with E[mt] <= 6k.
 */
template <typename T = int, typename I>
vector<array<T, 2>> choose_sample(int k, I a, I b) {
    assert(k <= 50'000'000); // don't try anything crazy
    if (k == 0 || a >= b - 1)
        return {};

    using sample_t = vector<array<T, 2>>;
    long univ = 1L * (b - a) * (b - a - 1) / 2;
    assert(univ >= 0 && 0 <= k && k <= univ);

    // 1/5 One sample -- sample any
    if (k == 1) {
        uniform_int_distribution<I> distx(a, b - 1), disty(a, b - 2);
        I x = distx(mt), y = disty(mt);
        tie(x, y) = minmax(x + 0, y + (y >= x));
        sample_t sample = {{x, y}};
        return sample;
    }

    // 2/5 Full sample -- run iota
    if (k == univ) {
        sample_t whole(univ);
        int i = 0;
        for (I x = a; x < b; x++) {
            for (I y = x + 1; y < b; y++) {
                whole[i++] = {x, y};
            }
        }
        return whole;
    }

    // 3/5 Majority sample: run negative bitset sampling
    if (k >= univ / 2) {
        vector<bool> unsampled(univ, false);
        int included = univ;
        intd disti(0, univ - 1);
        while (included > k) {
            int i = disti(mt);
            included -= !unsampled[i];
            unsampled[i] = true;
        }
        sample_t sample(k);
        I x = a, y = a + 1;
        for (int i = 0, n = 0; i < k; n++) {
            if (!unsampled[n]) {
                sample[i++] = {x, y};
            }
            tie(x, y) = y == b - 1 ? make_pair(x + 1, x + 2) : make_pair(x, y + 1);
        }
        return sample;
    }

    // 4/5 Large minority sample: run positive bitset sampling
    if (k >= univ / 24) {
        vector<bool> sampled(univ, false);
        int included = 0;
        intd disti(0, univ - 1);
        while (included < k) {
            int i = disti(mt);
            included += !sampled[i];
            sampled[i] = true;
        }
        sample_t sample(k);
        I x = a, y = a + 1;
        for (int i = 0, n = 0; i < k; n++) {
            if (sampled[n]) {
                sample[i++] = {x, y};
            }
            tie(x, y) = y == b - 1 ? make_pair(x + 1, x + 2) : make_pair(x, y + 1);
        }
        return sample;
    }

    // 5/5 Large sample: run repeated sampling
    sample_t sample(k);
    uniform_int_distribution<I> distx(a, b - 1), disty(a, b - 2);
    for (int i = 0; i < k; i++) {
        I x = distx(mt), y = disty(mt);
        tie(x, y) = minmax(x + 0, y + (y >= x));
        assert(x < y);
        sample[i] = {x, y};
    }
    sort(begin(sample), end(sample));
    int S = unique(begin(sample), end(sample)) - begin(sample);
    while (S < k) {
        int M = S;
        do {
            for (int i = M; i < k; i++) {
                I x = distx(mt), y = disty(mt);
                tie(x, y) = minmax(x + 0, y + (y >= x));
                assert(x < y);
                sample[i] = {x, y};
            }
            sort(begin(sample) + M, end(sample));
            inplace_merge(begin(sample) + S, begin(sample) + M, end(sample));
            M = unique(begin(sample) + S, end(sample)) - begin(sample);
        } while (M < k);

        inplace_merge(begin(sample), begin(sample) + S, end(sample));
        S = unique(begin(sample), end(sample)) - begin(sample);
    }
    return sample;
}

template <typename T = int, typename I>
auto choose_sample_p(double p, I a, I b) {
    long ab = 1L * (b - a) * (b - a - 1) / 2;
    return choose_sample<T>(binomd(ab, min(p, 1.0))(mt), a, b);
}

/**
 * Generate a sorted sample of k integer pairs (x,y) where x is taken from the range
 * [a..b) and y is taken from the range [c..d).
 * It must hold that a <= b, c <= d, and k <= nm = (b - a)(d - c).
 * Complexity: O(k log k) with E[mt] <= 6k.
 */
template <typename T = int, typename I>
vector<array<T, 2>> pair_sample(int k, I a, I b, I c, I d) {
    assert(k <= 50'000'000); // don't try anything crazy
    if (k == 0 || a >= b || c >= d)
        return {};

    using sample_t = vector<array<T, 2>>;
    long univ = 1L * (b - a) * (d - c);
    assert(univ >= 0 && 0 <= k && k <= univ);

    // 1/5 One sample -- sample any
    if (k == 1) {
        uniform_int_distribution<I> distx(a, b - 1), disty(c, d - 1);
        I x = distx(mt), y = disty(mt);
        sample_t sample = {{x, y}};
        return sample;
    }

    // 2/5 Full sample -- run iota
    if (k == univ) {
        sample_t whole(univ);
        int i = 0;
        for (I x = a; x < b; x++) {
            for (I y = c; y < d; y++) {
                whole[i++] = {x, y};
            }
        }
        return whole;
    }

    // 3/5 Majority sample: run negative bitset sampling
    if (k >= univ / 2) {
        vector<bool> unsampled(univ, false);
        int included = univ;
        intd disti(0, univ - 1);
        while (included > k) {
            int i = disti(mt);
            included -= !unsampled[i];
            unsampled[i] = true;
        }
        sample_t sample(k);
        I x = a, y = c;
        for (int i = 0, n = 0; i < k; n++) {
            if (!unsampled[n]) {
                sample[i++] = {x, y};
            }
            tie(x, y) = y == d - 1 ? make_pair(x + 1, c) : make_pair(x, y + 1);
        }
        return sample;
    }

    // 4/5 Large minority sample: run positive bitset sampling
    if (k >= univ / 24) {
        vector<bool> sampled(univ, false);
        int included = 0;
        intd disti(0, univ - 1);
        while (included < k) {
            int i = disti(mt);
            included += !sampled[i];
            sampled[i] = true;
        }
        sample_t sample(k);
        I x = a, y = c;
        for (int i = 0, n = 0; i < k; n++) {
            if (sampled[n]) {
                sample[i++] = {x, y};
            }
            tie(x, y) = y == d - 1 ? make_pair(x + 1, c) : make_pair(x, y + 1);
        }
        return sample;
    }

    // 5/5 Large sample: run repeated sampling
    sample_t sample(k);
    uniform_int_distribution<I> distx(a, b - 1), disty(c, d - 1);
    for (int i = 0; i < k; i++) {
        I x = distx(mt), y = disty(mt);
        sample[i] = {x, y};
    }
    sort(begin(sample), end(sample));
    int S = unique(begin(sample), end(sample)) - begin(sample);
    while (S < k) {
        int M = S;
        do {
            for (int i = M; i < k; i++) {
                I x = distx(mt), y = disty(mt);
                sample[i] = {x, y};
            }
            sort(begin(sample) + M, end(sample));
            inplace_merge(begin(sample) + S, begin(sample) + M, end(sample));
            M = unique(begin(sample) + S, end(sample)) - begin(sample);
        } while (M < k);

        inplace_merge(begin(sample), begin(sample) + S, end(sample));
        S = unique(begin(sample), end(sample)) - begin(sample);
    }
    return sample;
}

template <typename T = int, typename I>
auto pair_sample_p(double p, I a, I b, I c, I d) {
    long ab = b - a, cd = d - c;
    return pair_sample<T>(binomd(ab * cd, min(p, 1.0))(mt), a, b, c, d);
}

/**
 * Generate an unsorted sample of k integers pairs (x,y) where x and y are taken from the
 * range [a..b) and x != y.
 * It must hold that a <= b, and k <= n(n - 1) where n = b - a.
 * Complexity: O(k log k) with E[mt] <= 6k.
 */
template <typename T = int, typename I>
auto distinct_pair_sample(int k, I a, I b) {
    auto g = pair_sample(k, a, b, a, b - 1);
    for (auto& [u, v] : g)
        v += v >= u;
    return g;
}

template <typename T = int, typename I>
auto distinct_pair_sample_p(double p, I a, I b) {
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
auto parent_sample(int n, int zero = 0) {
    vector<int> parent(n + zero, zero);
    for (int i = 1 + zero; i < n + zero; i++) {
        intd dist(zero, i - 1);
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
    auto idx = int_sample(positives + negatives, 0, n);
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
