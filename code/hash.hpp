#include <bits/stdc++.h>

using namespace std;

// *****

struct vec_hasher {
    template <template <typename...> class Container, typename V, typename... T>
    size_t operator()(const Container<V, T...>& vec) const noexcept {
        hash<V> hasher;
        size_t seed = distance(begin(vec), end(vec));
        for (auto n : vec) {
            seed ^= hasher(n) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

struct pair_hasher {
    template <typename U, typename V>
    size_t operator()(const pair<U, V>& p) const noexcept {
        size_t lhs = p.first, rhs = p.second;
        return (lhs + rhs) * (lhs + rhs + 1) / 2 + rhs;
    }
};

constexpr size_t powmod(size_t base, size_t n, size_t mod) {
    size_t power = 1;
    while (n) {
        if (n & 1) {
            power = power * base % mod;
        }
        n >>= 1;
        base = base * base % mod;
    }
    return power;
}

template <size_t n>
struct rolling_hasher {
    static constexpr size_t base = 24'230'861UL;
    static constexpr size_t mod = 189'326'923UL;
    static constexpr size_t mul = powmod(base, n, mod);

    size_t operator()(const string& s) const noexcept {
        assert(s.size() == n);
        size_t seed = 0;
        for (unsigned char c : s) {
            seed = (seed * base + c) % mod;
        }
        return seed;
    }

    size_t roll(size_t seed, unsigned char out, unsigned char in) const noexcept {
        return (seed * base + in + (mod - out) * mul) % mod;
    }
};

// 9 8 7 6 5 4 3 2 1
