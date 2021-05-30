#ifndef POLY_HPP
#define POLY_HPP

#include "fft.hpp"
#include "ntt.hpp"
#include "../struct/y_combinator.hpp"

/**
 * Polynomial arithmetic operations and other functions
 * Requires FFT (and NTT if using modnum)
 * Reference: https://cp-algorithms.com/algebra/polynomial.html
 */

namespace polymath {

#define tmpl(T) template <typename T>

tmpl(T) T binpow(T val, long e) {
    T base = {1};
    while (e > 0) {
        if (e & 1)
            base *= val;
        if (e >>= 1)
            val *= val;
    }
    return base;
}

tmpl(T) void trim(vector<T>& a) {
    if constexpr (is_floating_point<T>::value)
        while (!a.empty() && abs(a.back()) < 30 * numeric_limits<T>::epsilon())
            a.pop_back();
    else
        while (!a.empty() && a.back() == T())
            a.pop_back();
}

tmpl(T) void truncate(vector<T>& v, int size) { v.resize(min(int(v.size()), size)); }

tmpl(T) auto truncated(vector<T> v, int size) { return truncate(v, size), v; }

tmpl(T) auto eval(const vector<T>& a, T x) {
    T v = 0;
    for (int A = a.size(), i = A - 1; i >= 0; i--)
        v = a[i] + v * x;
    return v;
}

tmpl(T) void deriv_inplace(vector<T>& a) {
    int N = a.size();
    for (int i = 0; i + 1 < N; i++)
        a[i] = T(i + 1) * a[i + 1];
    if (N > 0)
        a.pop_back();
}

tmpl(T) auto deriv(vector<T> a) {
    deriv_inplace(a);
    return a;
}

tmpl(T) void integr_inplace(vector<T>& a, T c = T()) {
    int N = a.size();
    a.resize(N + 1);
    for (int i = N; i > 0; i--)
        a[i] = a[i - 1] / T(i);
    a[0] = c;
}

tmpl(T) auto integr(vector<T> a, T c = T()) {
    integr_inplace(a, c);
    return a;
}

tmpl(T) auto withroots(const vector<T>& roots) {
    int R = roots.size();
    vector<vector<T>> polys(R);

    for (int i = 0; i < R; i++) {
        polys[i] = {-roots[i], 1};
    }

    while (R > 1) {
        for (int i = 0; i < R / 2; i++) {
            polys[i] = polys[i << 1] * polys[i << 1 | 1];
        }
        if (R & 1) {
            polys[R / 2] = move(polys[R - 1]);
        }
        R = (R + 1) / 2;
        polys.resize(R);
    }

    return polys[0];
}

tmpl(T) auto operator-(vector<T> a) {
    for (int A = a.size(), i = 0; i < A; i++)
        a[i] = -a[i];
    return a;
}

tmpl(T) auto& operator+=(vector<T>& a, const vector<T>& b) {
    int A = a.size(), B = b.size();
    a.resize(max(A, B));
    for (int i = 0; i < B; i++)
        a[i] += b[i];
    trim(a);
    return a;
}

tmpl(T) auto operator+(vector<T> a, const vector<T>& b) { return a += b; }

tmpl(T) auto& operator-=(vector<T>& a, const vector<T>& b) {
    int A = a.size(), B = b.size();
    a.resize(max(A, B));
    for (int i = 0; i < B; i++)
        a[i] -= b[i];
    trim(a);
    return a;
}

tmpl(T) auto operator-(vector<T> a, const vector<T>& b) { return a -= b; }

tmpl(T) auto& operator*=(vector<T>& a, T constant) {
    for (int i = 0, A = a.size(); i < A; i++)
        a[i] *= constant;
    return a;
}

tmpl(T) auto operator*(T constant, vector<T> a) { return a *= constant; }

tmpl(T) auto& operator/=(vector<T>& a, T constant) {
    for (int i = 0, A = a.size(); i < A; i++)
        a[i] /= constant;
    return a;
}

tmpl(T) auto operator/(vector<T> a, T constant) { return a /= constant; }

tmpl(T) auto& operator*=(vector<T>& a, const vector<T>& b) {
    return a = fft::fft_multiply(a, b);
}

tmpl(T) auto operator*(const vector<T>& a, const vector<T>& b) {
    return fft::fft_multiply(a, b);
}

tmpl(T) auto square(const vector<T>& a) { return fft::fft_square(a); }

tmpl(T) auto inverse_series(const vector<T>& a, int mod_degree) {
    assert(a[0]);
    vector<T> b(1, T(1) / a[0]);

    for (int len = 1; len < mod_degree; len *= 2) {
        // compute inverse series modulo x^len
        b += b - truncated(a, 2 * len) * square(b);
        truncate(b, min(2 * len, mod_degree)), trim(b);
    }

    return b;
}

tmpl(T) auto operator/(vector<T> a, vector<T> b) {
    int A = a.size(), B = b.size();
    if (B > A)
        return vector<T>();

    reverse(begin(a), end(a));
    reverse(begin(b), end(b));
    auto d = a * inverse_series(b, A - B + 1);
    truncate(d, A - B + 1);
    reverse(begin(d), end(d));
    return d;
}

tmpl(T) auto& operator/=(vector<T>& a, const vector<T>& b) { return a = a / b; }

tmpl(T) auto operator%(const vector<T>& a, const vector<T>& b) { return a - b * (a / b); }

tmpl(T) auto& operator%=(vector<T>& a, const vector<T>& b) { return a = a % b; }

tmpl(T) auto division_with_remainder(const vector<T>& a, const vector<T>& b) {
    auto d = a / b;
    auto r = a - b * d;
    return make_pair(move(d), move(r));
}

tmpl(T) auto gcd(const vector<T>& a, const vector<T>& b) -> vector<T> {
    return b.empty() ? a.empty() ? a : a / a.back() : gcd(b, a % b);
}

tmpl(T) auto resultant(const vector<T>& a, const vector<T>& b) {
    int A = a.size(), B = b.size();
    if (B == 0) {
        return T();
    } else if (B == 1) {
        return binpow(b[0], A - 1);
    } else {
        auto c = a % b;
        A -= c.size();
        auto mul = binpow(b[0], A - 1) * T(((A - 1) & (B - 1) & 1) ? -1 : 1);
        return mul * resultant(b, c);
    }
}

#undef tmpl

} // namespace polymath

#endif // POLY_HPP
