#ifndef NTT_HPP
#define NTT_HPP

#include "modnum.hpp"
#include "fft.hpp"

namespace fft {

constexpr int INTMOD_BREAKEVEN = 160;
constexpr int SPLITMOD_BREAKEVEN = 200;

inline namespace primitive {

int get_primitive_root(int p) {
    static unordered_map<int, int> cache = {{998244353, 3}};
    if (cache.count(p)) {
        return cache.at(p);
    }
    assert(false && "Sorry, unimplemented");
}

} // namespace primitive

inline namespace caches {

template <int mod>
struct root_of_unity<modnum<mod>> {
    using type = modnum<mod>;
    static type get(int n) {
        modnum<mod> g = get_primitive_root(mod);
        assert(n > 0 && (mod - 1) % n == 0 && "Modulus cannot handle NTT this large");
        return modpow(g, (mod - 1) / n);
    }
};

} // namespace caches

template <typename Mod, typename At, typename Bt, typename Ct>
void fft_split_lower_upper(Mod H, At* ia, int A, Bt* ismall, Ct* ilarge) {
    for (int i = 0; i < A; i++) {
        ismall[i] = ia[i] % H;
        ilarge[i] = ia[i] / H;
    }
}

inline namespace naive {

template <typename Prom, typename Mod, typename At, typename Bt, typename Ct>
void naive_multiply_mod_run(Mod mod, At* ia, int A, Bt* ib, int B, Ct* ic) {
    for (int i = 0; i < A; i++) {
        for (int j = 0; j < B; j++) {
            ic[i + j] += Prom(ia[i]) * Prom(ib[j]) % mod;
            if (ic[i + j] < 0)
                ic[i + j] += mod;
            else if (ic[i + j] >= mod)
                ic[i + j] -= mod;
        }
    }
}

template <typename Prom, typename Mod, typename At, typename Ct>
void naive_square_mod_run(Mod mod, At* ia, int A, Ct* ic) {
    for (int i = 0; i < A; i++) {
        for (int j = 0; j < A; j++) {
            ic[i + j] += Prom(ia[i]) * Prom(ia[j]) % mod;
            if (ic[i + j] < 0)
                ic[i + j] += mod;
            else if (ic[i + j] >= mod)
                ic[i + j] -= mod;
        }
    }
}

template <typename Prom = int64_t, typename Mod, typename T>
auto naive_multiply_mod(Mod mod, const vector<T>& a, const vector<T>& b) {
    int A = a.size(), B = b.size(), S = A && B ? A + B - 1 : 0;
    vector<T> c(S);
    naive_multiply_mod_run<Prom>(mod, a.data(), A, b.data(), B, c.data());
    return c;
}

template <typename Prom = int64_t, typename Mod, typename T>
auto naive_square_mod(Mod mod, const vector<T>& a) {
    int A = a.size(), S = A ? 2 * A - 1 : 0;
    vector<T> c(S);
    naive_square_mod_run<Prom>(mod, a.data(), A, c.data());
    return c;
}

} // namespace naive

template <int MOD>
auto fft_multiply(const vector<modnum<MOD>>& a, const vector<modnum<MOD>>& b) {
    int A = a.size(), B = b.size(), S = A && B ? A + B - 1 : 0;
    vector<modnum<MOD>> c(S);

    if (A <= INTMOD_BREAKEVEN || B <= INTMOD_BREAKEVEN) {
        naive_multiply_run(a.data(), A, b.data(), B, c.data());
    } else {
        fft_multiply_run<exact_ext, modnum<MOD>>(a.data(), A, b.data(), B, c.data());
    }

    return c;
}

template <int MOD>
auto fft_square(const vector<modnum<MOD>>& a) {
    int A = a.size(), S = A ? 2 * A - 1 : 0;
    vector<modnum<MOD>> c(S);

    if (A <= INTMOD_BREAKEVEN) {
        naive_square_run(a.data(), A, c.data());
    } else {
        fft_square_run<exact_ext, modnum<MOD>>(a.data(), A, c.data());
    }

    return c;
}

template <typename Prom = int64_t, typename C = default_complex, typename Mod, typename T>
auto fft_split_multiply(Mod mod, const vector<T>& a, const vector<T>& b) {
    int A = a.size(), B = b.size(), S = A && B ? A + B - 1 : 0;
    vector<T> c(S);
    if (A == 0 || B == 0)
        return c;

    if (A <= SPLITMOD_BREAKEVEN || B <= SPLITMOD_BREAKEVEN) {
        naive_multiply_mod_run<Prom>(mod, a.data(), A, b.data(), B, c.data());
        return c;
    }

    // a = a0 + H * a1    |    b = b0 + H * b1
    Mod H = sqrt(mod), Q = H * H % mod;
    vector<T> a0(A), a1(A), b0(B), b1(B);
    fft_split_lower_upper(H, a.data(), A, a0.data(), a1.data());
    fft_split_lower_upper(H, b.data(), B, b0.data(), b1.data());

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
        if (c[i] < 0)
            c[i] += mod;
        else if (c[i] >= mod)
            c[i] -= mod;
    }

    return c;
}

template <typename Prom = int64_t, typename C = default_complex, typename Mod, typename T>
auto fft_split_square(Mod mod, const vector<T>& a) {
    int A = a.size(), S = A ? 2 * A - 1 : 0;
    vector<T> c(S);
    if (A == 0)
        return c;

    if (A <= SPLITMOD_BREAKEVEN) {
        naive_square_mod_run<Prom>(mod, a.data(), A, c.data());
        return c;
    }

    // a = a0 + H * a1    |    b = b0 + H * b1
    Mod H = sqrt(mod), Q = H * H % mod;
    vector<T> a0(A), a1(A);
    fft_split_lower_upper(H, a.data(), A, a0.data(), a1.data());

    vector<Prom> c0(S), c1(S), c2(S);
    fft_multiply_run<int_ext, C>(a0.data(), A, a0.data(), A, c0.data());
    fft_multiply_run<int_ext, C>(a0.data(), A, a1.data(), A, c1.data());
    fft_multiply_run<int_ext, C>(a1.data(), A, a1.data(), A, c2.data());

    for (int i = 0; i < S; i++) {
        c[i] = c0[i] % mod;
        c[i] = (c[i] + 2 * (c1[i] % mod * H % mod)) % mod;
        c[i] = (c[i] + (c2[i] % mod * Q % mod)) % mod;
        if (c[i] < 0)
            c[i] += mod;
        else if (c[i] >= mod)
            c[i] -= mod;
    }

    return c;
}

} // namespace fft

#endif // NTT_HPP
