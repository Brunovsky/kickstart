#pragma once

#include "modnum.hpp"

/**
 * Generalized FFT-like fast convolutions in O(N log N) given by a matrix
 *                                 [ A  B ]
 *                                 [ C  D ]
 *
 * (1, 1, 1,-1) --> XOR (fwht)     c := a[i] b[j] x^(i XOR j)
 * (0, 1, 1, 1) --> AND            c := a[i] b[j] x^(i AND j)
 * (1, 1, 1, 0) --> OR             c := a[i] b[j] x^(i OR j)
 */
template <int A, int B, int C, int D, bool inverse, typename T>
void conv_transform(vector<T>& a) {
    int N = a.size();
    assert((N & (N - 1)) == 0);
    for (int h = 1; h < N; h *= 2) {
        for (int i = 0; i < N; i += 2 * h) {
            for (int j = i; j < i + h; j++) {
                auto x = a[j];
                auto y = a[j + h];
                if constexpr (inverse) {
                    a[j] = D * x - B * y;
                    a[j + h] = -C * x + A * y;
                } else {
                    a[j] = A * x + B * y;
                    a[j + h] = C * x + D * y;
                }
            }
        }
    }
    if constexpr (inverse && A * D - B * C != 1) {
        static_assert(A * D - B * C != 0);
        T div = 1;
        int n = N > 1 ? 8 * sizeof(N) - __builtin_clz(N - 1) : 0;
        if constexpr (A * D - B * C == -1) {
            div = (n & 1) ? -1 : 1;
        } else {
            while (n--) {
                div *= T(A * D - B * C);
            }
        }
        if (div != 1) {
            for (int i = 0; i < N; i++) {
                a[i] /= div;
            }
        }
    }
}

template <int A, int B, int C, int D, typename T>
auto convolve(const vector<T>& a, const vector<T>& b) {
    if (a.empty() || b.empty()) {
        return vector<T>();
    }
    int AS = a.size(), BS = b.size(), M = max(AS, BS);
    int N = 1 << (M > 1 ? 8 * sizeof(M) - __builtin_clz(M - 1) : 0);
    vector<T> c = a, d = b;
    c.resize(N), d.resize(N);
    conv_transform<A, B, C, D, 0>(c);
    conv_transform<A, B, C, D, 0>(d);
    for (int i = 0; i < N; i++) {
        c[i] = c[i] * d[i];
    }
    conv_transform<A, B, C, D, 1>(c);
    if constexpr (is_floating_point<T>::value)
        while (!c.empty() && abs(c.back()) <= 30 * numeric_limits<T>::epsilon())
            c.pop_back();
    else
        while (!c.empty() && c.back() == T(0))
            c.pop_back();
    return c;
}

template <typename T>
auto xor_convolution(const vector<T>& a, const vector<T>& b) {
    return convolve<1, 1, 1, -1>(a, b);
}

template <typename T>
auto and_convolution(const vector<T>& a, const vector<T>& b) {
    return convolve<0, 1, 1, 1>(a, b);
}

template <typename T>
auto or_convolution(const vector<T>& a, const vector<T>& b) {
    return convolve<1, 1, 1, 0>(a, b);
}

/**
 * Given f : [0..2^n) -> T, compute sum over subsets (aka SOS DP)
 * F[i]: SUM f[j], j∈i
 * Complexity: O(N log N) == O(n 2^n)
 */
template <typename T, typename O = T>
auto sos_subsets_convolution(const vector<T>& f) {
    int N = f.size(), n = N > 1 ? 8 * sizeof(N) - __builtin_clz(N - 1) : 0;
    assert(N == (1 << n));
    vector<O> F(begin(f), end(f));
    for (int i = 0; i < n; i++) {
        for (int bit = 1 << i, mask = 0; mask < N; mask++) {
            if (mask & bit) {
                F[mask] += F[mask ^ bit];
            }
        }
    }
    return F;
}

/**
 * Given f : [0..2^n) -> T, compute sum over supersets (aka SOS DP)
 * F[i]: SUM f[j], i∈j
 * Complexity: O(N log N) == O(n 2^n)
 */
template <typename T, typename O = T>
auto sos_supersets_convolution(const vector<T>& f) {
    int N = f.size(), n = N > 1 ? 8 * sizeof(N) - __builtin_clz(N - 1) : 0;
    assert(N == (1 << n));
    vector<O> F(begin(f), end(f));
    for (int i = 0; i < n; i++) {
        for (int bit = 1 << i, mask = 0; mask < N; mask++) {
            if (mask & bit) {
                F[mask ^ bit] += F[mask];
            }
        }
    }
    return F;
}

/**
 * Given f : [0..2^n) -> T, compute mobius sum over subsets (inverse of SOS)
 * F[i]: SUM (-1)^|i-j| f[j], j∈i
 * Complexity: O(N log N) == O(n 2^n)
 */
template <typename T, typename O = T>
auto mobius_subsets_convolution(const vector<T>& f) {
    int N = f.size(), n = N > 1 ? 8 * sizeof(N) - __builtin_clz(N - 1) : 0;
    assert(N == (1 << n));
    vector<O> F(begin(f), end(f));
    for (int i = 0; i < n; i++) {
        for (int bit = 1 << i, mask = 0; mask < N; mask++) {
            if (mask & bit) {
                F[mask] -= F[mask ^ bit];
            }
        }
    }
    return F;
}

/**
 * Given f : [0..2^n) -> T, compute mobius sum over supersets (inverse of SOS)
 * F[i]: SUM (-1)^|i-j| f[j], i∈j
 * Complexity: O(N log N) == O(n 2^n)
 */
template <typename T, typename O = T>
auto mobius_supersets_convolution(const vector<T>& f) {
    int N = f.size(), n = N > 1 ? 8 * sizeof(N) - __builtin_clz(N - 1) : 0;
    assert(N == (1 << n));
    vector<O> F(begin(f), end(f));
    for (int i = 0; i < n; i++) {
        for (int bit = 1 << i, mask = 0; mask < N; mask++) {
            if (mask & bit) {
                F[mask ^ bit] -= F[mask];
            }
        }
    }
    return F;
}

/**
 * Given f : [0..2^n) -> T and g : [0..2^n) -> T, compute subset sum convolution
 * S[i]: SUM f[a]g[b], a&b=0, a|b=i
 * Complexity: O(N log^2 N) == O(n^2 2^n)
 * Reference: https://codeforces.com/blog/entry/72488
 */
template <typename T, typename O = T>
auto subset_convolution(const vector<T>& f, const vector<T>& g) {
    int N = f.size(), n = N > 1 ? 8 * sizeof(N) - __builtin_clz(N - 1) : 0;
    assert(N == (1 << n) && N == int(g.size()));
    vector<vector<O>> fhat(n + 1, vector<O>(N));
    vector<vector<O>> ghat(n + 1, vector<O>(N));
    for (int mask = 0; mask < N; mask++) {
        int bits = mask ? __builtin_popcount(mask) : 0;
        fhat[bits][mask] = f[mask];
        ghat[bits][mask] = g[mask];
    }
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j < n; j++) {
            for (int bit = 1 << j, mask = 0; mask < N; mask++) {
                if (mask & bit) {
                    fhat[i][mask] += fhat[i][mask ^ bit];
                    ghat[i][mask] += ghat[i][mask ^ bit];
                }
            }
        }
    }
    vector<vector<O>> h(n + 1, vector<O>(N));
    for (int mask = 0; mask < N; mask++) {
        for (int i = 0; i <= n; i++) {
            for (int j = 0; j <= i; j++) {
                h[i][mask] += fhat[j][mask] * ghat[i - j][mask];
            }
        }
    }
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j < n; j++) {
            for (int bit = 1 << j, mask = 0; mask < N; mask++) {
                if (mask & bit) {
                    h[i][mask] -= h[i][mask ^ bit];
                }
            }
        }
    }
    vector<O> S(N);
    for (int mask = 0; mask < N; mask++) {
        int bits = mask ? __builtin_popcount(mask) : 0;
        S[mask] = h[bits][mask];
    }
    return S;
}
