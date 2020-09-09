#ifndef RANDOM_HPP
#define RANDOM_HPP

#include "hash.hpp"

using namespace std;

// *****

default_random_engine mt(random_device{}());
using intd = uniform_int_distribution<int>;
using longd = uniform_int_distribution<long>;
using ulongd = uniform_int_distribution<size_t>;
using reald = uniform_real_distribution<double>;
using binomd = binomial_distribution<int>;
using boold = bernoulli_distribution;

int different(int u, int v1, int v2) {
    assert(v1 <= v2 && (v1 != u || v2 != u));
    intd dist(v1, v2);
    while (true)
        if ((v1 = dist(mt)) != u)
            return v1;
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
vector<int> int_sample(int k, int a, int b, bool complement = false) {
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
 * Generate a sorted sample of k integer pairs (x,y) where x, y are taken from the range
 * [a..b] and x <= y if eq or x < y if !eq.
 * It must hold that a <= b and k <= (n choose 2) where n = b - a + 1.
 * Complexity: O(k) with E[mt] <= 6k.
 */
vector<array<int, 2>> choose_sample(int k, int a, int b, bool eq,
                                    bool complement = false) {
    long ab = 1L * (b - a + 1) * (b - a + 2) / 2 - !eq * (b - a + 1);
    assert(a <= b && 0 <= k && k <= ab);
    if (3 * k >= 2 * ab) {
        return choose_sample(ab - k, a, b, eq, !complement);
    }

    intd dist(a, b);
    unordered_set<pair<int, int>, pair_hasher> seen;
    seen.reserve(k);
    while (k--) {
        pair<int, int> p;
        do {
            p = minmax(dist(mt), dist(mt));
        } while ((!eq && p.first == p.second) || seen.count(p));
        seen.insert(p);
    }

    vector<array<int, 2>> sample;
    if (complement) {
        for (int x = a; x <= b; x++)
            for (int y = x + !eq; y <= b; y++)
                if (!seen.count({x, y}))
                    sample.push_back({x, y});
    } else if (3 * k >= b - a + 1) {
        for (int x = a; x <= b; x++)
            for (int y = x + !eq; y <= b; y++)
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
vector<array<int, 2>> pair_sample(int k, int a, int b, int c, int d,
                                  bool complement = false) {
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

    vector<array<int, 2>> sample;
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
vector<int> parent_sample(int n) {
    vector<int> parent(n);
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
vector<int> partition_sample(int n, int k, int m = 1) {
    vector<int> parts(k, m);
    intd dist(0, k - 1);
    n -= m * k;
    assert(n >= 0);
    while (n > 0) {
        int add = (n + k - 1) / k;
        parts[dist(mt)] += add;
        n -= add;
    }
    return parts;
}

/**
 * Like partition_sample but the first and last levels have size exactly 1.
 */
vector<int> partition_sample_flow(int V, int ranks, int m = 1) {
    auto R = partition_sample(V - 2, ranks - 2, m);
    R.insert(R.begin(), 1);
    R.insert(R.end(), 1);
    return R;
}

/**
 * Generate a k-regular undirected edge set on n vertices.
 * Requires nk even and k <= n - 1.
 * Complexity: O(kV * restarts)
 */
vector<array<int, 2>> regular_sample(int n, int k) {
    if (k == 0)
        return {};
    assert(3 <= k && k < n && !(k & 1 && n & 1));
    int restarts = 0, edges;

    unordered_set<array<int, 2>, vec_hasher> seen;
    vector<int> cnt(n), notfull;

restart:
    if (restarts++ == 500)
        throw std::runtime_error("Failed to generate regular graph after 500 restarts");

    seen.clear();
    notfull.resize(n);
    iota(begin(notfull), end(notfull), 0);
    fill(begin(cnt), end(cnt), k);
    edges = 0;

    while (!notfull.empty()) {
        int b = notfull.size();
        if (b == 1)
            goto restart;

        for (int i = 0; i < b; i++) {
            int u = notfull[i], v, j;
            int fast = 2 * b;
            intd other(0, b - 2);
            do { // fast loop, fallback to iterating through notfull otherwise
                j = other(mt);
                j += j >= i;
                v = notfull[j];
            } while (--fast && seen.count({u, v}));
            if (!fast && seen.count({u, v})) {
                for (j = 0; j < b; j++) {
                    v = notfull[j];
                    if (i != j && !seen.count({u, v}))
                        break;
                }
                if (j == b)
                    goto restart; // :(
            }

            cnt[u]--, cnt[v]--, edges++;
            seen.insert({u, v}), seen.insert({v, u});
            if (i < j) // careful not to invalidate the other node's index
                swap(u, v), swap(i, j);
            if (cnt[u] == 0)
                swap(notfull[i], notfull.back()), notfull.pop_back(), b--;
            if (cnt[v] == 0)
                swap(notfull[j], notfull.back()), notfull.pop_back(), b--;
        }
    }

    vector<array<int, 2>> regular(edges);
    for (auto p : seen)
        if (p[0] < p[1])
            regular[--edges] = p;
    assert(edges == 0);
    return regular;
}

vector<array<int, 2>> regular_bipartite_sample(int n, int m, int k) {
    if (k == 0)
        return {};
    assert(k <= m && n * k % m == 0);
    int restarts = 0, edges, x = n * k / m;

    unordered_set<array<int, 2>, vec_hasher> seen;
    vector<int> cnt(m), notfull;

restart:
    if (restarts++ == 500)
        throw std::runtime_error("Failed to generate regular graph after 500 restarts");

    seen.clear();
    notfull.resize(m);
    iota(begin(notfull), end(notfull), 0);
    fill(begin(cnt), end(cnt), x);
    edges = 0;

    for (int i = 0; i < k; i++) {
        for (int u = 0; u < n; u++) {
            intd distv(0, m - 1);
            int v, j, fast = 2 * m;
            do { // fast loop, fallback to iterating through notfull otherwise
                j = distv(mt);
                v = notfull[j];
            } while (--fast && seen.count({u, v}));
            if (!fast && seen.count({u, v})) {
                for (j = 0; j < m; j++) {
                    v = notfull[j];
                    if (i != j && !seen.count({u, v}))
                        break;
                }
                if (j == m)
                    goto restart; // :(
            }

            cnt[v]--, edges++;
            seen.insert({u, v});
            if (cnt[v] == 0)
                swap(notfull[j], notfull.back()), notfull.pop_back(), m--;
        }
    }

    vector<array<int, 2>> regular(edges);
    for (auto p : seen)
        regular[--edges] = p;
    assert(edges == 0);
    return regular;
}

#endif // RANDOM_HPP
