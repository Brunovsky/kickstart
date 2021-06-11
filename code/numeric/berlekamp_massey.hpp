#ifndef BERLEKAMP_MASSEY_HPP
#define BERLEKAMP_MASSEY_HPP

#include <bits/stdc++.h>
using namespace std;

/**
 * Consider an (unknown) <=n-order linear recurrence (LR) given by a[0..n) such that
 *            x[n+k+1] = a[0]x[n+k] + a[1]x[n+k-1] + ... + a[n-1]x[k+1]
 * with some given initial values.
 *
 * Recover the linear recurrence given some of its output.
 * In general, for an LR of expected size N, the output (x) should have size >N
 * to ensure that a meaningful LR was produced.
 *
 * Complexity: O(N^2), N=|x|.
 * Reference: kactl, https://crypto.stanford.edu/~mironov/cs359/massey.pdf
 */
template <typename T>
auto berlekamp_massey(const vector<T>& values) {
    int N = values.size(), L = 0, m = 1;
    vector<T> C(N), B(N);
    C[0] = B[0] = 1;

    T b = 1;
    for (int i = 0; i < N; i++, m++) {
        T d = values[i];
        for (int j = 1; j < L + 1; j++) d += C[j] * values[i - j];
        if (!d) continue;
        vector<T> tmp = C;
        T coef = d / b;
        for (int j = m; j < N; j++) C[j] -= coef * B[j - m];
        if (2 * L > i) continue;
        L = i + 1 - L, B = move(tmp), b = d, m = 0;
    }

    C.resize(L + 1), C.erase(C.begin());
    for (auto& n : C) n = -n;
    return C;
}

/**
 * Compute k-th term (1-indexed) linear recurrence rec with given starting values.
 * Complexity: O(N^2 log k). Computation table can be saved for a fixed recurrence.
 * Reference: ecnerwala
 */
template <typename T>
auto linear_recurrence(const vector<T>& values, const vector<T>& rec, int64_t k) {
    int N = int(rec.size());
    assert(values.size() >= rec.size());

    auto combine = [&](vector<T>& a, const vector<T>& b) {
        vector<T> res(2 * N + 1);
        for (int i = 0; i <= N; i++)
            for (int j = 0; j <= N; j++) res[i + j] += a[i] * b[j];
        for (int i = 2 * N; i > N; --i)
            for (int j = 0; j < N; j++) res[i - 1 - j] += res[i] * rec[j];
        res.resize(N + 1);
        a = move(res);
    };

    vector<T> pol(N + 1), e(N + 1);
    pol[0] = e[1] = 1;

    k++; // use 1-indexed kth
    while (k > 0) {
        if (k & 1) combine(pol, e);
        if (k >>= 1) combine(e, e);
    }

    T res = 0;
    for (int i = 0; i < N; i++) res += pol[i + 1] * values[i];
    return res;
}

#endif // BERLEKAMP_MASSEY_HPP
