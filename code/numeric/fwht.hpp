#pragma once

#include "modnum.hpp"

/**
 * Fast Walsh-Hadamard transform
 * Complexity: O(N log N), output is trimmed, with fast paths for small cases
 * Usage:
 *   vector<num> a(U), b(V);
 *   auto c = fwht_convolve(a, b);         // c := SUM_i,j a[i]b[j] x^(i XOR j)
 */
namespace fwht {

int next_two(int32_t N) { return N > 1 ? 8 * sizeof(N) - __builtin_clz(N - 1) : 0; }
constexpr int FWHT_BREAKEVEN = 100;

template <typename T>
void trim_vector(vector<T>& v) {
    if constexpr (is_floating_point<T>::value)
        while (!v.empty() && abs(v.back()) <= 30 * numeric_limits<T>::epsilon())
            v.pop_back();
    else
        while (!v.empty() && v.back() == T(0))
            v.pop_back();
}

template <typename T>
auto fwht_naive_convolve(const vector<T>& a, const vector<T>& b) {
    int A = a.size(), B = b.size(), N = 1 << max(next_two(A), next_two(B));
    vector<T> c(N);
    for (int i = 0; i < A && B; i++)
        for (int j = 0; j < B; j++)
            c[i ^ j] += a[i] * b[j];
    trim_vector(c);
    return c;
}

template <bool inverse, typename T>
auto fwht_transform(vector<T>& a) {
    for (int h = 1, N = a.size(); h < N; h *= 2) {
        for (int i = 0; i < N; i += 2 * h) {
            for (int j = i; j < i + h; j++) {
                auto x = a[j];
                auto y = a[j + h];
                a[j] = x + y;
                a[j + h] = x - y;
            }
        }
    }
    if constexpr (inverse) {
        for (int i = 0, N = a.size(); i < N; i++) {
            a[i] /= T(N);
        }
    }
}

template <typename T>
auto fwht_convolve(const vector<T>& a, const vector<T>& b) {
    if (a.empty() || b.empty()) {
        return vector<T>();
    }
    int A = a.size(), B = b.size(), N = 1 << max(next_two(A), next_two(B));
    if (A <= FWHT_BREAKEVEN || B <= FWHT_BREAKEVEN) {
        return fwht_naive_convolve(a, b);
    }
    vector<T> c = a, d = b;
    c.resize(N), d.resize(N);
    fwht_transform<0>(c);
    fwht_transform<0>(d);
    for (int i = 0; i < N; i++) {
        c[i] = c[i] * d[i];
    }
    fwht_transform<1>(c);
    trim_vector(c);
    return c;
}

} // namespace fwht
