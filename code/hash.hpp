#include <bits/stdc++.h>

using namespace std;

// *****

struct vechasher {
    template <template <typename...> typename Container, typename V, typename... T>
    size_t operator()(const Container<V, T...>& vec) const noexcept {
        hash<V> hasher;
        size_t seed = distance(begin(vec), end(vec));
        for (auto n : vec) {
            seed ^= hasher(n) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

struct pairhasher {
    template <typename U, typename V>
    size_t operator()(const pair<U, V>& p) const noexcept {
        size_t lhs = p.first, rhs = p.second;
        return (lhs + rhs) * (lhs + rhs + 1) / 2 + rhs;
    }
};
