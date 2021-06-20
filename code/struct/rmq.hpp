#pragma once

#include <bits/stdc++.h>
using namespace std;

/**
 * Range Minimum Query
 * Source: kth (https://github.com/kth-competitive-programming/kactl)
 * For maximum just exchange min() for max().
 * Complexity: O(V log V) construction, O(1) query
 */
template <typename T>
struct min_rmq {
    vector<vector<T>> jmp;

    min_rmq() = default;
    explicit min_rmq(const vector<T>& v) : jmp(1, v) {
        for (int len = 1, k = 1, N = v.size(); 2 * len <= N; len *= 2, ++k) {
            int J = N - 2 * len + 1;
            jmp.emplace_back(J);
            for (int j = 0; j < J; j++) {
                const auto& l = jmp[k - 1][j];
                const auto& r = jmp[k - 1][j + len];
                jmp[k][j] = l < r ? l : r; // prefers last
            }
        }
    }

    T query(int a, int b) const /* [a,b) */ {
        static constexpr int BITS = CHAR_BIT * sizeof(int) - 1;
        assert(a < b); // or return inf if a == b
        int bits = BITS - __builtin_clz(b - a);
        const auto& l = jmp[bits][a];
        const auto& r = jmp[bits][b - (1 << bits)];
        return l < r ? l : r;
    }
};

template <typename T>
struct min_rmq_index {
    vector<T> v;
    vector<vector<int>> jmp;

    min_rmq_index() = default;
    explicit min_rmq_index(const vector<T>& v) : v(v), jmp(1, vector<int>(v.size())) {
        iota(begin(jmp[0]), end(jmp[0]), 0);
        for (int len = 1, k = 1, N = v.size(); 2 * len <= N; len *= 2, ++k) {
            int J = N - 2 * len + 1;
            jmp.emplace_back(J);
            for (int j = 0; j < J; j++) {
                int l = jmp[k - 1][j];
                int r = jmp[k - 1][j + len];
                jmp[k][j] = v[l] < v[r] ? l : r; // prefers last
            }
        }
    }

    int query(int a, int b) const /* [a, b) */ {
        static constexpr int BITS = CHAR_BIT * sizeof(int) - 1;
        assert(a < b); // or return inf if a == b
        int bits = BITS - __builtin_clz(b - a);
        int l = jmp[bits][a];
        int r = jmp[bits][b - (1 << bits)];
        return v[l] < v[r] ? l : r;
    }
};
