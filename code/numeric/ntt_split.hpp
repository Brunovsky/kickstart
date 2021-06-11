#pragma once

#include "ntt.hpp"

namespace fft {

constexpr int SPLITMOD_BREAKEVEN = 200;

inline namespace detail {

template <typename Mod, typename T, typename OT>
void fft_split_lower_upper_mod(Mod H, const T* a, int A, OT* ismall, OT* ilarge) {
    for (int i = 0; i < A; i++) {
        ismall[i] = a[i] % H;
        ilarge[i] = a[i] / H;
    }
}

template <typename Prom, typename Mod, typename T, typename OT>
void naive_multiply_mod_run(Mod mod, const T* a, int A, const T* b, int B, OT* c) {
    for (int i = 0; i < A && B; i++) {
        for (int j = 0; j < B; j++) {
            c[i + j] += Prom(a[i]) * Prom(b[j]) % mod;
            c[i + j] += c[i + j] < 0 ? mod : c[i + j] >= mod ? -mod : 0;
        }
    }
}

template <typename Prom, typename Mod, typename T, typename OT>
void naive_square_mod_run(Mod mod, const T* a, int A, OT* c) {
    for (int i = 0; i < A; i++) {
        for (int j = 0; j < A; j++) {
            c[i + j] += Prom(a[i]) * Prom(a[j]) % mod;
            c[i + j] += c[i + j] < 0 ? mod : c[i + j] >= mod ? -mod : 0;
        }
    }
}

} // namespace detail

template <typename Prom = int64_t, typename C = default_complex, typename Mod, typename T>
auto fft_multiply_mod(Mod mod, const vector<T>& a, const vector<T>& b) {
    int A = a.size(), B = b.size(), S = A && B ? A + B - 1 : 0;
    vector<T> c(S);
    if (S == 0)
        return c;

    if (A <= SPLITMOD_BREAKEVEN || B <= SPLITMOD_BREAKEVEN) {
        naive_multiply_mod_run<Prom>(mod, a.data(), A, b.data(), B, c.data());
        return c;
    }

    // a = a0 + H * a1    |    b = b0 + H * b1
    Mod H = sqrt(mod), Q = H * H % mod;
    vector<T> a0(A), a1(A), b0(B), b1(B);
    fft_split_lower_upper_mod(H, a.data(), A, a0.data(), a1.data());
    fft_split_lower_upper_mod(H, b.data(), B, b0.data(), b1.data());

    vector<Prom> c00(S), c01(S), c10(S), c11(S);
    fft_multiply_run<int_ext, C>(a0.data(), A, b0.data(), B, c00.data());
    fft_multiply_run<int_ext, C>(a0.data(), A, b1.data(), B, c01.data());
    fft_multiply_run<int_ext, C>(a1.data(), A, b0.data(), B, c10.data());
    fft_multiply_run<int_ext, C>(a1.data(), A, b1.data(), B, c11.data());

    for (int i = 0; i < S; i++) {
        c[i] = c00[i] % mod;
        c[i] = (c[i] + (c10[i] % mod * H % mod)) % mod;
        c[i] = (c[i] + (c01[i] % mod * H % mod)) % mod;
        c[i] = (c[i] + (c11[i] % mod * Q % mod)) % mod;
        c[i] += c[i] < 0 ? mod : c[i] >= mod ? -mod : 0;
    }

    return c;
}

template <typename Prom = int64_t, typename C = default_complex, typename Mod, typename T>
auto fft_square_mod(Mod mod, const vector<T>& a) {
    int A = a.size(), S = A ? 2 * A - 1 : 0;
    vector<T> c(S);
    if (S == 0)
        return c;

    if (A <= SPLITMOD_BREAKEVEN) {
        naive_square_mod_run<Prom>(mod, a.data(), A, c.data());
        return c;
    }

    // a = a0 + H * a1    |    b = b0 + H * b1
    Mod H = sqrt(mod), Q = H * H % mod;
    vector<T> a0(A), a1(A);
    fft_split_lower_upper_mod(H, a.data(), A, a0.data(), a1.data());

    vector<Prom> c0(S), c1(S), c2(S);
    fft_multiply_run<int_ext, C>(a0.data(), A, a0.data(), A, c0.data());
    fft_multiply_run<int_ext, C>(a0.data(), A, a1.data(), A, c1.data());
    fft_multiply_run<int_ext, C>(a1.data(), A, a1.data(), A, c2.data());

    for (int i = 0; i < S; i++) {
        c[i] = c0[i] % mod;
        c[i] = (c[i] + 2 * (c1[i] % mod * H % mod)) % mod;
        c[i] = (c[i] + (c2[i] % mod * Q % mod)) % mod;
        c[i] += c[i] < 0 ? mod : c[i] >= mod ? -mod : 0;
    }

    return c;
}

} // namespace fft
