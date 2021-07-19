#pragma once

#include <bits/stdc++.h>
using namespace std;

template <typename T>
auto interpolate_iota(const vector<T>& y, int64_t x) {
    int N = y.size();
    if (0 <= x && x < N) {
        return y[x];
    } else if (N == 0) {
        return T(0);
    }

    vector<T> invfac(N);
    T fac = 1;
    for (int i = 2; i < N; i++) {
        fac *= i;
    }
    invfac[N - 1] = T(1) / fac;
    for (int i = N - 2; i >= 0; i--) {
        invfac[i] = invfac[i + 1] * (i + 1);
    }

    vector<T> dp(N, 1), pd(N, 1);
    for (int i = 0; i < N - 1; i++) {
        dp[i + 1] = dp[i] * (x - i);
    }
    for (int i = N - 1; i > 0; i--) {
        pd[i - 1] = pd[i] * (x - i);
    }

    T val = 0;
    for (int i = 0; i < N; i++) {
        T cell = y[i] * dp[i] * pd[i] * invfac[i] * invfac[N - i - 1];
        val += ((N - i) & 1) ? cell : -cell;
    }
    return val;
}
