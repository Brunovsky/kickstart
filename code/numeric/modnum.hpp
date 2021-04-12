#ifndef MODNUM_HPP
#define MODNUM_HPP

#include "math.hpp"

// *****

template <long mod>
struct modnum {
    long n;

    static long fit(long v) { return v >= mod ? v - mod : (v < 0 ? v + mod : v); }

    modnum() : n(0) {}
    modnum(long v) : n(fit(v % mod)) {}
    explicit operator long() const { return n; }
    modnum& operator+=(modnum v) { return n = fit(n + v.n), *this; }
    modnum& operator-=(modnum v) { return n = fit(n - v.n), *this; }
    modnum& operator*=(modnum v) { return n = (n * v.n) % mod, *this; }
    modnum& operator/=(modnum v) { return n = (n * invmod(v.n, mod)) % mod, *this; }
    friend modnum operator+(modnum lhs, modnum rhs) { return lhs += rhs; }
    friend modnum operator-(modnum lhs, modnum rhs) { return lhs -= rhs; }
    friend modnum operator*(modnum lhs, modnum rhs) { return lhs *= rhs; }
    friend modnum operator/(modnum lhs, modnum rhs) { return lhs /= rhs; }
    friend bool operator==(modnum lhs, modnum rhs) { return lhs.n == rhs.n; }
    friend bool operator!=(modnum lhs, modnum rhs) { return lhs.n != rhs.n; }
};

struct dmodnum {
    long n, mod;

    static long fit(long v, long mod) { return v = v % mod, v >= 0 ? v : v + mod; }

    dmodnum(long v) : n(v), mod(0) {}
    dmodnum(long v, long mod) : n(fit(v, mod)), mod(mod) {}
    explicit operator long() const { return n; }
    dmodnum& operator+=(dmodnum v) { return n = fit(n + v.n, mod), *this; }
    dmodnum& operator-=(dmodnum v) { return n = fit(n - v.n, mod), *this; }
    dmodnum& operator*=(dmodnum v) { return n = (n * v.n) % mod, *this; }
    dmodnum& operator/=(dmodnum v) { return n = (n * invmod(v.n, mod)) % mod, *this; }
    friend dmodnum operator+(dmodnum lhs, dmodnum rhs) { return lhs += rhs; }
    friend dmodnum operator-(dmodnum lhs, dmodnum rhs) { return lhs -= rhs; }
    friend dmodnum operator*(dmodnum lhs, dmodnum rhs) { return lhs *= rhs; }
    friend dmodnum operator/(dmodnum lhs, dmodnum rhs) { return lhs /= rhs; }
    friend bool operator==(dmodnum lhs, dmodnum rhs) { return lhs.n == rhs.n; }
    friend bool operator!=(dmodnum lhs, dmodnum rhs) { return lhs.n != rhs.n; }
};

#endif // MODNUM_HPP
