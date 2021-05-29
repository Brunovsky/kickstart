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
        a[i] = (i + 1) * a[i + 1];
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
        a[i] = a[i - 1] / i;
    a[0] = c;
}

tmpl(T) auto integr(vector<T> a, T c = T()) {
    integr_inplace(a, c);
    return a;
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
    vector<T> b(1, 1 / a[0]);
    int prev = 0; // size of b in the previous iteration
    int curr = 1; // size of b after this iteration
    for (int len = 1; prev < curr && len < mod_degree; len *= 2) {
        // compute inverse series modulo x^len
        prev = curr;
        b += b - truncated(a, 2 * len) * square(b);
        trim(b), truncate(b, min(2 * len, mod_degree));
        curr = b.size();
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

tmpl(T) struct multieval_tree {
    vector<array<int, 2>> ranges;
    vector<vector<T>> tree;
    vector<T> x;
};

tmpl(T) auto build_multieval_tree(const vector<T>& x) {
    int N = x.size(), M = 1 << fft::next_two(N);

    // We build a perfect segtree (no wasted nodes)
    // Build leaves, then align leaves, then pushup nodes

    vector<array<int, 2>> ranges(2 * N);
    vector<vector<T>> tree(2 * N);
    for (int i = 0; i < N; i++) {
        ranges[i + N] = {i, i + 1};
        tree[i + N] = {-x[i], 1};
    }

    rotate(begin(ranges) + N, begin(ranges) + (3 * N - M), end(ranges));
    rotate(begin(tree) + N, begin(tree) + (3 * N - M), end(tree));

    for (int i = N - 1; i >= 1; i--) {
        int l = i << 1, r = i << 1 | 1;
        ranges[i] = {ranges[l][0], ranges[r][1]};
        assert(ranges[i][0] + 1 < ranges[i][1]);
        tree[i] = tree[l] * tree[r];
        assert(int(tree[i].size()) == ranges[i][1] - ranges[i][0] + 1);
    }

    return multieval_tree<T>{move(ranges), move(tree), x};
}

tmpl(T) void multieval_dfs(int i, const vector<T>& b, vector<T>& value,
                           const multieval_tree<T>& evaltree) {
    const auto& [ranges, tree, x] = evaltree;
    auto [L, R] = ranges[i];
    if (L + 1 == R) {
        value[L] = eval(b, x[L]);
    } else {
        int l = i << 1, r = i << 1 | 1;
        multieval_dfs(l, b % tree[l], value, evaltree);
        multieval_dfs(r, b % tree[r], value, evaltree);
    }
}

tmpl(T) auto multieval(const vector<T>& a, const multieval_tree<T>& evaltree) {
    vector<T> value(evaltree.x.size());
    multieval_dfs(1, a % evaltree.tree[1], value, evaltree);
    return value;
}

tmpl(T) auto multieval(const vector<T>& a, const vector<T>& x) {
    return multieval(a, build_multieval_tree(x));
}

} // namespace polymath

#endif // POLY_HPP
