#ifndef HASH_HPP
#define HASH_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

struct vec_hasher {
    template <typename Container>
    size_t operator()(const Container& vec) const noexcept {
        using std::hash;
        using T = typename Container::value_type;
        hash<T> hasher;
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
    template <typename U>
    size_t operator()(const array<U, 2>& p) const noexcept {
        size_t lhs = p[0], rhs = p[1];
        return (lhs + rhs) * (lhs + rhs + 1) / 2 + rhs;
    }
};

struct rolling_hasher {
    static constexpr size_t base = 2001539UL;
    static constexpr size_t mask = (1 << 26) - 1;
    size_t n, mul;

    rolling_hasher(size_t n) : n(n), mul(powovf(n) & mask) {}

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

    constexpr size_t powovf(size_t e) {
        size_t power = 1, b = base;
        while (e) {
            if (e & 1) {
                power = power * b;
            }
            e >>= 1;
            b = b * b;
        }
        return power;
    }
};

#endif // HASH_HPP
