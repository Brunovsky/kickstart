#pragma once

#include <bits/stdc++.h>
using namespace std;

template <uint32_t mod>
struct modnum {
    using u32 = uint32_t;
    using u64 = uint64_t;
    static_assert(mod > 0 && mod < UINT_MAX / 2);

    uint32_t n;

    constexpr modnum() : n(0) {}
    constexpr modnum(u64 v) : n(v >= mod ? v % mod : v) {}
    constexpr modnum(u32 v) : n(v >= mod ? v % mod : v) {}
    constexpr modnum(int64_t v) : modnum(v >= 0 ? u64(v) : u64(mod + v % int(mod))) {}
    constexpr modnum(int32_t v) : modnum(v >= 0 ? u32(v) : u32(mod + v % int(mod))) {}
    explicit constexpr operator int() const { return n; }
    explicit constexpr operator bool() const { return n != 0; }

    static constexpr u32 fit(u32 x) { return x >= mod ? x - mod : x; }
    static constexpr int modinv(u32 x) {
        int nx = 1, ny = 0;
        u32 y = mod;
        while (x) {
            auto k = y / x;
            y = y % x;
            ny = ny - k * nx;
            swap(x, y), swap(nx, ny);
        }
        return ny < 0 ? mod + ny : ny;
    }
    friend constexpr modnum modpow(modnum b, long e) {
        modnum p = 1;
        while (e > 0) {
            if (e & 1)
                p = p * b;
            if (e >>= 1)
                b = b * b;
        }
        return p;
    }

    constexpr modnum inv() const { return modinv(n); }
    constexpr modnum operator-() const { return n == 0 ? n : mod - n; }
    constexpr modnum operator+() const { return *this; }
    constexpr modnum operator++(int) { return n = fit(n + 1), *this - 1; }
    constexpr modnum operator--(int) { return n = fit(mod + n - 1), *this + 1; }
    constexpr modnum &operator++() { return n = fit(n + 1), *this; }
    constexpr modnum &operator--() { return n = fit(mod + n - 1), *this; }
    constexpr modnum &operator+=(modnum v) { return n = fit(n + v.n), *this; }
    constexpr modnum &operator-=(modnum v) { return n = fit(mod + n - v.n), *this; }
    constexpr modnum &operator*=(modnum v) { return n = (u64(n) * v.n) % mod, *this; }
    constexpr modnum &operator/=(modnum v) { return *this *= v.inv(); }

    friend constexpr modnum operator+(modnum lhs, modnum rhs) { return lhs += rhs; }
    friend constexpr modnum operator-(modnum lhs, modnum rhs) { return lhs -= rhs; }
    friend constexpr modnum operator*(modnum lhs, modnum rhs) { return lhs *= rhs; }
    friend constexpr modnum operator/(modnum lhs, modnum rhs) { return lhs /= rhs; }

    friend string to_string(modnum v) { return to_string(v.n); }
    friend constexpr bool operator==(modnum lhs, modnum rhs) { return lhs.n == rhs.n; }
    friend constexpr bool operator!=(modnum lhs, modnum rhs) { return lhs.n != rhs.n; }
    friend ostream &operator<<(ostream &out, modnum v) { return out << v.n; }
    friend istream &operator>>(istream &in, modnum &v) {
        int64_t n;
        return in >> n, v = modnum(n);
    }
};

template <uint32_t mod>
struct montg {
    using u32 = uint32_t;
    using u64 = uint64_t;
    static_assert(mod > 0 && mod < UINT_MAX / 2);
    static constexpr u32 get_r() {
        u32 ret = mod;
        for (int i = 0; i < 4; ++i)
            ret *= 2 - mod * ret;
        return ret;
    }
    static constexpr u32 r = get_r();
    static constexpr u32 n2 = -u64(mod) % mod;

    u32 a;

    constexpr montg() : a(0) {}
    constexpr montg(u64 b) : a(reduce(u64(b % mod + mod) * n2)) {}
    constexpr montg(u32 b) : a(reduce(u64(b % mod + mod) * n2)) {}
    constexpr montg(int64_t b) : a(reduce(u64(b % mod + mod) * n2)) {}
    constexpr montg(int32_t b) : a(reduce(u64(b % mod + mod) * n2)) {}
    explicit operator int() const { return a; }
    explicit operator bool() const { return a != 0; }

    static constexpr u32 fit(u32 v) { return v >= mod ? v - mod : v; }
    static constexpr u32 reduce(const u64 &b) {
        return (b + u64(u32(b) * u32(-r)) * mod) >> 32;
    }
    friend constexpr montg modpow(montg mul, u64 n) {
        montg ret(1);
        while (n > 0) {
            if (n & 1)
                ret *= mul;
            if (n >>= 1)
                mul *= mul;
        }
        return ret;
    }

    constexpr montg inv() const { return modpow(*this, mod - 2); }
    constexpr montg operator-() const { return montg() - montg(*this); }
    constexpr montg operator+() const { return *this; }
    constexpr montg &operator+=(montg b) {
        if (int(a += b.a - 2 * mod) < 0)
            a += 2 * mod;
        return *this;
    }
    constexpr montg &operator-=(montg b) {
        if (int(a -= b.a) < 0)
            a += 2 * mod;
        return *this;
    }
    constexpr montg &operator*=(montg b) { return a = reduce(u64(a) * b.a), *this; }
    constexpr montg &operator/=(montg b) { return *this *= b.inv(); }

    friend constexpr montg operator+(montg lhs, montg rhs) { return lhs += rhs; }
    friend constexpr montg operator-(montg lhs, montg rhs) { return lhs -= rhs; }
    friend constexpr montg operator*(montg lhs, montg rhs) { return lhs *= rhs; }
    friend constexpr montg operator/(montg lhs, montg rhs) { return lhs /= rhs; }

    constexpr bool operator==(const montg &b) const { return fit(a) == fit(b.a); }
    constexpr bool operator!=(const montg &b) const { return fit(a) != fit(b.a); }
    friend ostream &operator<<(ostream &out, const montg &b) { return out << b.get(); }
    friend istream &operator>>(istream &in, montg &b) {
        int64_t t;
        return in >> t, b = montg(t), in;
    }
    constexpr u32 get() const {
        u32 ret = reduce(a);
        return ret >= mod ? ret - mod : ret;
    }
};
