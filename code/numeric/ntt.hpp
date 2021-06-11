#pragma once

#include "modnum.hpp"
#include "fft.hpp"

namespace fft {

constexpr int MODNUM_BREAKEVEN = 160;

inline namespace detail {

int get_primitive_root(int p) {
    static unordered_map<int, int> cache = {{998244353, 3}};
    if (cache.count(p)) {
        return cache.at(p);
    }
    assert(false && "Sorry, unimplemented");
}

template <int mod>
struct root_of_unity<modnum<mod>> {
    using type = modnum<mod>;
    static type get(int n) {
        modnum<mod> g = get_primitive_root(mod);
        assert(n > 0 && (mod - 1) % n == 0 && "Modulus cannot handle NTT this large");
        return modpow(g, (mod - 1) / n);
    }
};

} // namespace detail

template <int MOD>
auto fft_multiply(const vector<modnum<MOD>>& a, const vector<modnum<MOD>>& b) {
    int A = a.size(), B = b.size(), S = A && B ? A + B - 1 : 0;
    vector<modnum<MOD>> c(S);
    if (S == 0)
        return c;

    if (A <= MODNUM_BREAKEVEN || B <= MODNUM_BREAKEVEN) {
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
    if (S == 0)
        return c;

    if (A <= MODNUM_BREAKEVEN) {
        naive_square_run(a.data(), A, c.data());
    } else {
        fft_square_run<exact_ext, modnum<MOD>>(a.data(), A, c.data());
    }

    return c;
}

} // namespace fft
