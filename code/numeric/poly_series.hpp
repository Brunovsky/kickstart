#pragma once

#include "poly.hpp"

namespace polymath {

template <typename T>
auto falling_factorial(int n) {
    vector<T> roots(n);
    iota(begin(roots), end(roots), T(0));
    return withroots(roots);
}

template <typename T>
auto rising_factorial(int n) {
    vector<T> roots(n);
    iota(begin(roots), end(roots), T(1 - n));
    return withroots(roots);
}

/**
 * Compute faul[0..n] such that faul[i] = 1^i + 2^i + ... + m^i
 * Complexity: O(n log n) or O(n^2)
 */
template <typename T>
auto faulhaber(T m, int n) {
    vector<T> ex(n + 1);
    T e = 1;
    for (int i = 0; i <= n; i++) {
        ex[i] = e;
        e /= i + 1;
    }
    vector<T> den = ex;
    den.erase(den.begin());
    for (auto& d : den) {
        d = -d;
    }
    vector<T> num(n);
    T p = 1;
    for (int i = 0; i < n; i++) {
        p *= m + 1;
        num[i] = ex[i + 1] * (1 - p);
    }
    vector<T> faul = truncated(num * inverse_series(den, n), n);
    T f = 1;
    for (int i = 0; i < n; i++) {
        faul[i] *= f;
        f *= i + 1;
    }
    return faul;
}

/**
 * Compute the Nth row of stirling numbers of the first kind.
 * stirling_1st[k] = number of permutations with k disjoint cycles.
 * Complexity: O(n log n) or O(n^2)
 */
template <typename T>
auto stirling_1st(int n) {
    vector<T> roots(n); // same as rising factorial
    iota(begin(roots), end(roots), T(1 - n));
    return withroots(roots);
}

/**
 * Compute the Nth row of the stirling numbers of the second kind.
 * stirling_2nd[k] = number of partitions of n elements into k nonempty disjoint groups.
 * Complexity: O(n log n) or O(n^2)
 */
template <typename T>
auto stirling_2nd(int n) {
    // convolve a[i] = (-1)^i / i!  with  b[i] = i^n / i!
    vector<T> a(n + 1), b(n + 1);
    a[0] = T(1);
    T f = 1;
    for (int i = 1; i <= n; i++) {
        f /= T(i);
        a[i] = (i & 1) ? -f : f;
        b[i] = binpow(i, n) * f;
    }
    return n ? truncated(a * b, n + 1) : vector<T>{T(1)};
}

} // namespace polymath
