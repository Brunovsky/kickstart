#include <bits/stdc++.h>

using namespace std;

// *****

struct vec_hasher {
    template <template <typename...> class Container, typename V, typename... T>
    size_t operator()(const Container<V, T...>& vec) const noexcept {
        using std::hash;
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

struct rolling_hasher {
    static constexpr size_t base = 2001539UL;
    static constexpr size_t mask = (1 << 26) - 1;
    size_t n, mul;

    rolling_hasher(size_t n) : n(n), mul(powovf(base, n) & mask) {}

    size_t operator()(const char* s, const char* e) const noexcept {
        size_t seed = 0;
        while (s != e) {
            seed = (seed * base + *s++) & mask;
        }
        return seed;
    }

    size_t operator()(const string& s) const noexcept {
        return (*this)(s.data(), s.data() + s.length());
    }

    size_t roll(size_t seed, unsigned char out, unsigned char in) const noexcept {
        return (seed * base + in + (mask + 1 - out) * mul) & mask;
    }

    constexpr size_t powovf(size_t base, size_t n) {
        size_t power = 1;
        while (n) {
            if (n & 1) {
                power = power * base;
            }
            n >>= 1;
            base = base * base;
        }
        return power;
    }
};
