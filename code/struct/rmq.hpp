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
    static constexpr int bits = CHAR_BIT * sizeof(int) - 1;

    min_rmq() = default;
    explicit min_rmq(const vector<T>& v) : jmp(1, v) {
        for (unsigned len = 1, k = 1; len * 2 <= v.size(); len *= 2, ++k) {
            jmp.emplace_back(v.size() - len * 2 + 1);
            for (unsigned j = 0; j < jmp[k].size(); j++) {
                const auto& l = jmp[k - 1][j];
                const auto& r = jmp[k - 1][j + len];
                jmp[k][j] = min(l, r);
            }
        }
    }

    T query(int a, int b) const /* [a,b) */ {
        assert(a < b); // or return inf if a == b
        int dep = bits - __builtin_clz(b - a);
        const auto& l = jmp[dep][a];
        const auto& r = jmp[dep][b - (1 << dep)];
        return min(l, r);
    }
};

template <typename T>
struct min_rmq_index {
    vector<T> v;
    vector<vector<int>> jmp;
    static constexpr int bits = CHAR_BIT * sizeof(int) - 1;

    min_rmq_index() = default;
    explicit min_rmq_index(const vector<T>& v) : v(v), jmp(1, vector<int>(v.size())) {
        iota(begin(jmp[0]), end(jmp[0]), 0);
        for (unsigned len = 1, k = 1; len * 2 <= v.size(); len *= 2, ++k) {
            jmp.emplace_back(v.size() - len * 2 + 1);
            for (unsigned j = 0; j < jmp[k].size(); j++) {
                int l = jmp[k - 1][j];
                int r = jmp[k - 1][j + len];
                jmp[k][j] = min(make_pair(v[l], l), make_pair(v[r], r)).second;
            }
        }
    }

    int query(int a, int b) const /* [a, b) */ {
        assert(a < b); // or return inf if a == b
        int dep = bits - __builtin_clz(b - a);
        int l = jmp[dep][a];
        int r = jmp[dep][b - (1 << dep)];
        return min(make_pair(v[l], l), make_pair(v[r], r)).second;
    }
};
