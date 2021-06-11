#pragma once

#include "fft.hpp"

namespace fft {

constexpr int SPLIT_BREAKEVEN = 1500;

template <typename T, typename At, typename Bt, typename Ct>
void fft_split_lower_upper(T H, At* ia, int A, Bt* ismall, Ct* ilarge) {
    for (int i = 0; i < A; i++) {
        ilarge[i] = floor(ia[i] / H);
        ismall[i] = ia[i] - ilarge[i] * H;
    }
}

template <typename C = default_complex, typename T>
auto fft_split_multiply(const vector<T>& a, const vector<T>& b) {
    int A = a.size(), B = b.size(), S = A && B ? A + B - 1 : 0;
    vector<T> c(S);
    if (A == 0 || B == 0)
        return c;

    if (A <= SPLIT_BREAKEVEN || B <= SPLIT_BREAKEVEN) {
        naive_multiply_run(a.data(), A, b.data(), B, c.data());
        return c;
    }

    T maxabs = max(abs(a[0]), abs(b[0]));
    for (int i = 1; i < A; i++)
        maxabs = max(maxabs, abs(a[i]));
    for (int i = 1; i < B; i++)
        maxabs = max(maxabs, abs(b[i]));

    // a = a0 + H * a1    |    b = b0 + H * b1
    T H = sqrt(maxabs), Q = H * H;
    vector<T> a0(A), a1(A), b0(B), b1(B);
    fft_split_lower_upper(H, a.data(), A, a0.data(), a1.data());
    fft_split_lower_upper(H, b.data(), B, b0.data(), b1.data());

    vector<T> c00(S), c01(S), c10(S), c11(S);
    if constexpr (is_integral<T>::value) {
        fft_multiply_run<int_ext, C>(a0.data(), A, b0.data(), B, c00.data());
        fft_multiply_run<int_ext, C>(a0.data(), A, b1.data(), B, c01.data());
        fft_multiply_run<int_ext, C>(a1.data(), A, b0.data(), B, c10.data());
        fft_multiply_run<int_ext, C>(a1.data(), A, b1.data(), B, c11.data());
    } else {
        fft_multiply_run<real_ext, C>(a0.data(), A, b0.data(), B, c00.data());
        fft_multiply_run<real_ext, C>(a0.data(), A, b1.data(), B, c01.data());
        fft_multiply_run<real_ext, C>(a1.data(), A, b0.data(), B, c10.data());
        fft_multiply_run<real_ext, C>(a1.data(), A, b1.data(), B, c11.data());
    }

    for (int i = 0; i < S; i++) {
        c[i] = c00[i] + (c10[i] + c01[i]) * H + c11[i] * Q;
    }

    return c;
}

template <typename C = default_complex, typename T>
auto fft_split_square(const vector<T>& a) {
    int A = a.size(), S = A ? 2 * A - 1 : 0;
    vector<T> c(S);
    if (A == 0)
        return c;

    if (A <= SPLIT_BREAKEVEN) {
        naive_square_run(a.data(), A, c.data());
        return c;
    }

    T maxabs = abs(a[0]);
    for (int i = 1; i < A; i++)
        maxabs = max(maxabs, abs(a[i]));

    // a = a0 + H * a1    |    b = b0 + H * b1
    T H = sqrt(maxabs), Q = H * H;
    vector<T> a0(A), a1(A);
    fft_split_lower_upper(H, a.data(), A, a0.data(), a1.data());

    vector<T> c0(S), c1(S), c2(S);
    if constexpr (is_integral<T>::value) {
        fft_multiply_run<int_ext, C>(a0.data(), A, a0.data(), A, c0.data());
        fft_multiply_run<int_ext, C>(a0.data(), A, a1.data(), A, c1.data());
        fft_multiply_run<int_ext, C>(a1.data(), A, a1.data(), A, c2.data());
    } else {
        fft_multiply_run<real_ext, C>(a0.data(), A, a0.data(), A, c0.data());
        fft_multiply_run<real_ext, C>(a0.data(), A, a1.data(), A, c1.data());
        fft_multiply_run<real_ext, C>(a1.data(), A, a1.data(), A, c2.data());
    }

    for (int i = 0; i < S; i++) {
        c[i] = c0[i] + c1[i] * H + c2[i] * Q;
    }

    return c;
}

} // namespace fft
