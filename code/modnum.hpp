#include "math.hpp"

// *****

template <long mod>
struct modnum {
    long n;

    static long fit(long v) { return v >= mod ? v - mod : (v < 0 ? v + mod : v); }

    modnum() : n(0) {}
    modnum(long v) : n(fit(v % mod)) {}
    explicit operator long() const { return n; }

    modnum& operator+=(modnum v) {
        n += v.n;
        if (n >= mod)
            n -= mod;
        return *this;
    }
    modnum& operator-=(modnum v) {
        n -= v.n;
        if (n < 0)
            n += mod;
        return *this;
    }
    modnum& operator*=(modnum v) {
        n = (n * v.n) % mod;
        return *this;
    }
    modnum& operator/=(modnum v) {
        n = (n * invmod(v.n, mod)) % mod;
        return *this;
    }

    friend bool operator==(modnum lhs, modnum rhs) { return lhs.n == rhs.n; }
    friend bool operator!=(modnum lhs, modnum rhs) { return lhs.n != rhs.n; }
    friend modnum operator+(modnum lhs, modnum rhs) { return lhs += rhs; }
    friend modnum operator-(modnum lhs, modnum rhs) { return lhs -= rhs; }
    friend modnum operator*(modnum lhs, modnum rhs) { return lhs *= rhs; }
    friend modnum operator/(modnum lhs, modnum rhs) { return lhs /= rhs; }
};
