#ifndef RMQ_HPP
#define RMQ_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

/**
 * Range Minimum Query
 * Source: kth (https://github.com/kth-competitive-programming/kactl)
 * For maximum just exchange min() for max().
 * Complexity: O(V log V) construction, O(1) query
 */
template <typename T>
struct RMQ {
    vector<vector<T>> jmp;

    RMQ() = default;
    RMQ(const vector<T>& V) : jmp(1, V) {
        for (uint pw = 1, k = 1; pw * 2 <= V.size(); pw *= 2, ++k) {
            jmp.emplace_back(V.size() - pw * 2 + 1);
            for (uint j = 0; j < jmp[k].size(); j++)
                jmp[k][j] = min(jmp[k - 1][j], jmp[k - 1][j + pw]);
        }
    }

    T query(int a, int b) /* [a,b) */ {
        assert(a < b); // or return inf if a == b
        static constexpr int bits = CHAR_BIT * sizeof(int) - 1;
        int dep = bits - __builtin_clz(b - a);
        return min(jmp[dep][a], jmp[dep][b - (1 << dep)]);
    }
};

#endif // RMQ_HPP
