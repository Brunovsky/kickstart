#pragma once

#include "fft.hpp"
#include "../struct/y_combinator.hpp"

/**
 * Elementary polynomial arithmetic operations and other functions
 * Use either naive multiplication or a variant of FFT.
 * Fill in the multiplication/square functions below.
 * Reference: https://cp-algorithms.com/algebra/polynomial.html
 */
namespace polymath {

template <typename T>
auto multiply(const vector<T>& a, const vector<T>& b) {
    return fft::fft_multiply(a, b);
}

template <typename T>
T binpow(T val, long e) {
    T base = {1};
    while (e > 0) {
        if (e & 1)
            base *= val;
        if (e >>= 1)
            val *= val;
    }
    return base;
}

template <typename T>
void trim(vector<T>& a) {
    if constexpr (is_floating_point<T>::value)
        while (!a.empty() && abs(a.back()) < 30 * numeric_limits<T>::epsilon())
            a.pop_back();
    else
        while (!a.empty() && a.back() == T())
            a.pop_back();
}

template <typename T>
void truncate(vector<T>& v, int size) {
    v.resize(min(int(v.size()), size));
}

template <typename T>
auto truncated(vector<T> v, int size) {
    return truncate(v, size), v;
}

template <typename T>
auto& operator*=(vector<T>& a, const vector<T>& b) {
    return a = multiply(a, b);
}

template <typename T>
auto operator*(const vector<T>& a, const vector<T>& b) {
    return multiply(a, b);
}

template <typename T>
auto operator-(vector<T> a) {
    for (int A = a.size(), i = 0; i < A; i++)
        a[i] = -a[i];
    return a;
}

template <typename T>
auto& operator+=(vector<T>& a, const vector<T>& b) {
    int A = a.size(), B = b.size();
    a.resize(max(A, B));
    for (int i = 0; i < B; i++)
        a[i] += b[i];
    trim(a);
    return a;
}

template <typename T>
auto operator+(vector<T> a, const vector<T>& b) {
    return a += b;
}

template <typename T>
auto& operator-=(vector<T>& a, const vector<T>& b) {
    int A = a.size(), B = b.size();
    a.resize(max(A, B));
    for (int i = 0; i < B; i++)
        a[i] -= b[i];
    trim(a);
    return a;
}

template <typename T>
auto operator-(vector<T> a, const vector<T>& b) {
    return a -= b;
}

template <typename T>
auto& operator*=(vector<T>& a, T constant) {
    for (int i = 0, A = a.size(); i < A; i++)
        a[i] *= constant;
    return a;
}

template <typename T>
auto operator*(T constant, vector<T> a) {
    return a *= constant;
}

template <typename T>
auto& operator/=(vector<T>& a, T constant) {
    for (int i = 0, A = a.size(); i < A; i++)
        a[i] /= constant;
    return a;
}

template <typename T>
auto operator/(vector<T> a, T constant) {
    return a /= constant;
}

template <typename T>
auto eval(const vector<T>& a, T x) {
    T v = 0;
    for (int A = a.size(), i = A - 1; i >= 0; i--)
        v = a[i] + v * x;
    return v;
}

template <typename T>
auto deriv(vector<T> a) {
    int N = a.size();
    for (int i = 0; i + 1 < N; i++)
        a[i] = T(i + 1) * a[i + 1];
    if (N > 0)
        a.pop_back();
    return a;
}

template <typename T>
auto integr(vector<T> a, T c = T()) {
    int N = a.size();
    a.resize(N + 1);
    for (int i = N; i > 0; i--)
        a[i] = a[i - 1] / T(i);
    a[0] = c;
    return a;
}

template <typename T>
auto withroots(const vector<T>& roots) {
    int R = roots.size();
    vector<vector<T>> polys(R);

    for (int i = 0; i < R; i++) {
        polys[i] = {-roots[i], T(1)};
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

    return R ? polys[0] : vector<T>{T(1)};
}

template <typename T>
auto inverse_series(const vector<T>& a, int mod_degree) {
    assert(!a.empty() && a[0]);
    vector<T> b(1, T(1) / a[0]);

    for (int len = 1; len < mod_degree; len *= 2) {
        b += b - truncated(a, 2 * len) * (b * b);
        truncate(b, min(2 * len, mod_degree)), trim(b);
    }

    return b;
}

template <typename T>
auto operator/(vector<T> a, vector<T> b) {
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

template <typename T>
auto& operator/=(vector<T>& a, const vector<T>& b) {
    return a = a / b;
}

template <typename T>
auto operator%(const vector<T>& a, const vector<T>& b) {
    return a - b * (a / b);
}

template <typename T>
auto& operator%=(vector<T>& a, const vector<T>& b) {
    return a = a % b;
}

template <typename T>
auto division_with_remainder(const vector<T>& a, const vector<T>& b) {
    auto d = a / b, r = a - b * d;
    return make_pair(move(d), move(r));
}

template <typename T>
auto gcd(const vector<T>& a, const vector<T>& b) -> vector<T> {
    return b.empty() ? a.empty() ? a : a / a.back() : gcd(b, a % b);
}

template <typename T>
auto resultant(const vector<T>& a, const vector<T>& b) {
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

template <typename T>
struct multieval_tree {
    vector<int> index;
    vector<vector<T>> tree;
    vector<T> x;
};

template <typename T>
auto build_multieval_tree(const vector<T>& x) {
    int N = x.size(), M = 1 << fft::next_two(N);
    vector<int> index(N);
    vector<vector<T>> tree(2 * N);
    for (int i = 0; i < N; i++) {
        index[i] = i;
        tree[i + N] = {-x[i], 1};
    }

    rotate(begin(index), begin(index) + (2 * N - M), end(index));
    rotate(begin(tree) + N, begin(tree) + (3 * N - M), end(tree));

    for (int i = N - 1; i >= 1; i--) {
        int l = i << 1, r = i << 1 | 1;
        tree[i] = tree[l] * tree[r];
    }

    return multieval_tree<T>{move(index), move(tree), x};
}

template <typename T>
void multieval_dfs(int i, const vector<T>& poly, vector<T>& value,
                   const multieval_tree<T>& evaltree) {
    const auto& [index, tree, x] = evaltree;
    if (int N = x.size(); i >= N) {
        int j = index[i - N];
        value[j] = eval(poly, x[j]);
    } else {
        int l = i << 1, r = i << 1 | 1;
        multieval_dfs(l, poly % tree[l], value, evaltree);
        multieval_dfs(r, poly % tree[r], value, evaltree);
    }
}

template <typename T>
auto multieval(const vector<T>& poly, const multieval_tree<T>& evaltree) {
    vector<T> value(evaltree.x.size());
    multieval_dfs(1, poly % evaltree.tree[1], value, evaltree);
    return value;
}

template <typename T>
auto multieval(const vector<T>& poly, const vector<T>& x) {
    return multieval(poly, build_multieval_tree(x));
}

template <typename T>
auto interpolate_dfs(int i, const vector<T>& poly, const vector<T>& y,
                     const multieval_tree<T>& evaltree) {
    const auto& [index, tree, x] = evaltree;
    if (int N = x.size(); i >= N) {
        int j = index[i - N];
        return vector<T>{y[j] / poly[0]};
    } else {
        int l = i << 1, r = i << 1 | 1;
        auto a = interpolate_dfs(l, poly % tree[l], y, evaltree);
        auto b = interpolate_dfs(r, poly % tree[r], y, evaltree);
        return a * tree[r] + b * tree[l];
    }
}

template <typename T>
auto interpolate(const vector<T>& x, const vector<T>& y) {
    assert(x.size() == y.size());
    auto evaltree = build_multieval_tree(x);
    return interpolate_dfs(1, deriv(evaltree.tree[1]), y, evaltree);
}

} // namespace polymath
