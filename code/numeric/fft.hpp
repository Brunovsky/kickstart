#pragma once

#include "modnum.hpp"

// Base include
namespace fft {

template <typename T>
struct my_complex {
    using self = my_complex<T>;
    T x, y;
    constexpr my_complex(T x = T(0), T y = T(0)) : x(x), y(y) {}

    constexpr T& real() { return x; }
    constexpr T& imag() { return y; }
    constexpr const T& real() const { return x; }
    constexpr const T& imag() const { return y; }
    constexpr void real(T v) { x = v; }
    constexpr void imag(T v) { y = v; }
    constexpr friend auto real(self a) { return a.x; }
    constexpr friend auto imag(self a) { return a.y; }
    constexpr self rot_ccw(self a) { return self(-a.y, a.x); }
    constexpr self rot_cw(self a) { return self(a.y, -a.x); }
    constexpr friend auto abs(self a) { return sqrt(norm(a)); }
    constexpr friend auto arg(self a) { return atan2(a.y, a.x); }
    constexpr friend auto norm(self a) { return a.x * a.x + a.y * a.y; }
    constexpr friend auto conj(self a) { return self(a.x, -a.y); }
    constexpr friend auto inv(self a) { return self(a.x / norm(a), -a.y / norm(a)); }
    constexpr friend auto polar(T r, T theta = T()) {
        return self(r * cos(theta), r * sin(theta));
    }
    constexpr T& operator[](int i) { assert(i == 0 || i == 1), *(&x + i); }
    constexpr const T& operator[](int i) const { assert(i == 0 || i == 1), *(&x + i); }

    constexpr self& operator+=(self b) { return *this = *this + b; }
    constexpr self& operator-=(self b) { return *this = *this - b; }
    constexpr self& operator*=(self b) { return *this = *this * b; }
    constexpr self& operator/=(self b) { return *this = *this / b; }

    constexpr friend self operator*(self a, T b) { return self(a.x * b, a.y * b); }
    constexpr friend self operator*(T a, self b) { return self(a * b.x, a * b.y); }
    constexpr friend self operator/(self a, T b) { return self(a.x / b, a.y / b); }
    constexpr friend self operator/(T a, self b) { return a * inv(b); }
    constexpr friend self operator+(self a) { return self(a.x, a.y); }
    constexpr friend self operator-(self a) { return self(-a.x, -a.y); }
    constexpr friend self operator+(self a, self b) { return self(a.x + b.x, a.y + b.y); }
    constexpr friend self operator-(self a, self b) { return self(a.x - b.x, a.y - b.y); }
    constexpr friend self operator/(self a, self b) { return a * inv(b); }
    constexpr friend self operator*(self a, self b) {
        return self(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
    }
};

using default_complex = my_complex<double>;
constexpr double TAU = 6.283185307179586476925286766559;

constexpr auto operator""_i(long double x) { return default_complex(0, x); }

int next_two(int32_t N) { return N > 1 ? 8 * sizeof(N) - __builtin_clz(N - 1) : 0; }

template <typename T, typename D>
constexpr T fft_round(D coefficient) {
    return is_integral<T>::value ? llround(coefficient) : coefficient;
}

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
auto naive_multiply(const vector<T>& a, const vector<T>& b) {
    int A = a.size(), B = b.size(), C = A && B ? A + B - 1 : 0;
    vector<T> c(C);
    for (int i = 0; i < A && B; i++)
        for (int j = 0; j < B; j++)
            c[i + j] += a[i] * b[j];
    trim_vector(c);
    return c;
}

struct fft_reverse_cache {
    static inline vector<vector<int>> rev;

    static const int* get(int N) {
        int n = next_two(N), r = rev.size();
        rev.resize(max(r, n + 1));
        if (rev[n].empty()) {
            int R = 1 << n;
            rev[n].assign(R, 0);
            for (int i = 0; i < N; i++) {
                rev[n][i] = (rev[n][i >> 1] | ((i & 1) * R)) >> 1;
            }
        }
        return rev[n].data();
    }
};

template <typename T>
struct root_of_unity {};

template <typename D>
struct root_of_unity<my_complex<D>> {
    static auto get(int n) {
        assert(n > 0);
        return my_complex<D>(cos(TAU / n), sin(TAU / n));
    }
};

template <typename C>
struct fft_roots_cache {
    static inline vector<C> root = vector<C>(2, C(1));
    static inline vector<C> invroot = vector<C>(2, C(1));
    static inline vector<C> scratch_a, scratch_b;

    static auto get(int N) {
        for (int k = root.size(); k < N; k *= 2) {
            root.resize(2 * k);
            invroot.resize(2 * k);
            auto z = root_of_unity<C>::get(2 * k);
            auto iz = C(1) / z;
            for (int i = k / 2; i < k; i++) {
                root[2 * i] = root[i];
                root[2 * i + 1] = root[i] * z;
                invroot[2 * i] = invroot[i];
                invroot[2 * i + 1] = invroot[i] * iz;
            }
        }
        return make_pair(cref(root), cref(invroot));
    }

    static auto get_scratch(int N) {
        if (int(scratch_a.size()) < N) {
            scratch_a.resize(N);
            scratch_b.resize(N);
        }
        return make_pair(ref(scratch_a), ref(scratch_b));
    }
};

template <bool inverse, bool reverse, typename T>
void fft_transform(vector<T>& a, int N) {
    if constexpr (reverse) {
        auto rev = fft_reverse_cache::get(N);
        for (int i = 0; i < N; i++) {
            if (i < rev[i]) {
                swap(a[i], a[rev[i]]);
            }
        }
    }
    auto [root, invroot] = fft_roots_cache<T>::get(N);
    for (int k = 1; k < N; k *= 2) {
        for (int i = 0; i < N; i += 2 * k) {
            for (int l = i, r = i + k, j = 0; j < k; j++, l++, r++) {
                auto z = inverse ? invroot[j + k] : root[j + k];
                auto t = a[r] * z;
                a[r] = a[l] - t;
                a[l] = a[l] + t;
            }
        }
    }
    if constexpr (inverse) {
        auto inv = T(1) / T(N);
        for (int i = 0; i < N; i++) {
            a[i] *= inv;
        }
    }
}

} // namespace fft

// FFT with complex numbers
namespace fft {

constexpr int INT4_BREAKEVEN = 850;
constexpr int INT8_BREAKEVEN = 230;
constexpr int DOUBLE_BREAKEVEN = 400;

// (a+bi)(c+di) = (ac-bd)+(ad+bc)i
// c=0 => -bd + adi = d(-b + ai) =
template <typename T>
bool fft_small_breakeven(int N) {
    return (is_integral<T>::value && sizeof(T) <= 4 && N <= INT4_BREAKEVEN) ||
           (is_integral<T>::value && sizeof(T) > 4 && N <= INT8_BREAKEVEN) ||
           (is_floating_point<T>::value && N <= DOUBLE_BREAKEVEN);
}

template <typename C = default_complex, typename T>
auto fft_multiply(const vector<T>& a, const vector<T>& b) {
    if (a.empty() || b.empty()) {
        return vector<T>();
    }
    int A = a.size(), B = b.size();
    if (fft_small_breakeven<T>(A) || fft_small_breakeven<T>(B)) {
        return naive_multiply(a, b);
    }

    int S = A + B - 1, N = 1 << next_two(S), K = max(A, B);
    auto [fa, fb] = fft_roots_cache<C>::get_scratch(N);
    for (int i = 0; i < min(A, B); i++)
        fa[i] = C(a[i], b[i]);
    for (int i = B; i < A; i++)
        fa[i] = C(a[i], 0);
    for (int i = A; i < B; i++)
        fa[i] = C(0, b[i]);
    fill_n(begin(fa) + K, N - K, C(0));
    fft_transform<0, 1>(fa, N);
    for (int i = 0, j = 0; i < N; i++, j = N - i) {
        fb[i] = (fa[i] * fa[i] - conj(fa[j] * fa[j])) * -0.25_i;
    }
    fft_transform<1, 1>(fb, N);
    vector<T> c(S);
    for (int i = 0; i < S; i++) {
        c[i] = fft_round<T>(fb[i].real());
    }
    trim_vector(c);
    return c;
}

template <typename C = default_complex, typename T>
auto fft_square(const vector<T>& a) {
    if (a.empty()) {
        return vector<T>();
    }
    int A = a.size();
    if (fft_small_breakeven<T>(A)) {
        return naive_multiply(a, a);
    }

    int S = 2 * A - 1, N = 1 << next_two(S);
    auto [fa, fb] = fft_roots_cache<C>::get_scratch(N);
    copy_n(begin(a), A, begin(fa));
    fill_n(begin(fa) + A, N - A, C(0));
    fft_transform<0, 1>(fa, N);
    for (int i = 0; i < N; i++) {
        fa[i] = fa[i] * fa[i];
    }
    fft_transform<1, 1>(fa, N);
    vector<T> c(S);
    for (int i = 0; i < S; i++) {
        c[i] = fft_round<T>(fa[i].real());
    }
    trim_vector(c);
    return c;
}

} // namespace fft

// NTT with modnums
namespace fft {

constexpr int MODNUM_BREAKEVEN = 150;

int ntt_primitive_root(int p) {
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
        modnum<mod> g = ntt_primitive_root(mod);
        assert(n > 0 && (mod - 1) % n == 0 && "Modulus cannot handle NTT this large");
        return modpow(g, (mod - 1) / n);
    }
};

template <int MOD>
auto fft_multiply(const vector<modnum<MOD>>& a, const vector<modnum<MOD>>& b) {
    using T = modnum<MOD>;
    if (a.empty() || b.empty()) {
        return vector<T>();
    }
    int A = a.size(), B = b.size();
    if (A <= MODNUM_BREAKEVEN || B <= MODNUM_BREAKEVEN) {
        return naive_multiply(a, b);
    }

    int C = A + B - 1, N = 1 << next_two(C);
    vector<T> c = a, d = b;
    c.resize(N, T(0));
    d.resize(N, T(0));
    fft_transform<0, 1>(c, N);
    fft_transform<0, 1>(d, N);
    for (int i = 0; i < N; i++) {
        c[i] = c[i] * d[i];
    }
    fft_transform<1, 1>(c, N);
    trim_vector(c);
    return c;
}

template <int MOD>
auto fft_square(const vector<modnum<MOD>>& a) {
    using T = modnum<MOD>;
    if (a.empty()) {
        return vector<T>();
    }
    int A = a.size();
    if (A <= MODNUM_BREAKEVEN) {
        return naive_multiply(a, a);
    }

    int C = 2 * A - 1, N = 1 << next_two(C);
    vector<T> c = a;
    c.resize(N, T(0));
    fft_transform<0, 1>(c, N);
    for (int i = 0; i < N; i++) {
        c[i] = c[i] * c[i];
    }
    fft_transform<1, 1>(c, N);
    trim_vector(c);
    return c;
}

} // namespace fft

// FFT-SPLIT
namespace fft {

constexpr int SPLIT_BREAKEVEN = 700;

template <typename T>
auto maxabsolute(const vector<T>& a) {
    T maxabs = 0;
    for (const T& n : a)
        maxabs = max(maxabs, abs(n));
    return maxabs;
}

template <typename T, typename C>
void fft_split_lower_upper(T d, const vector<T>& a, vector<C>& comp) {
    for (int i = 0, A = a.size(); i < A; i++) {
        comp[i].imag(floor(a[i] / d));
        comp[i].real(a[i] - comp[i].imag() * d);
    }
}

template <typename C = default_complex, typename T>
auto fft_split_multiply(const vector<T>& a, const vector<T>& b) {
    if (a.empty() || b.empty()) {
        return vector<T>();
    }
    int A = a.size(), B = b.size();
    if (A <= SPLIT_BREAKEVEN || B <= SPLIT_BREAKEVEN) {
        return naive_multiply(a, b);
    }

    int S = A + B - 1, N = 1 << next_two(S);
    T H = sqrt(max(maxabsolute(a), maxabsolute(b))), Q = H * H;
    vector<C> ac(N), bc(N);
    fft_split_lower_upper(H, a, ac);
    fft_split_lower_upper(H, b, bc);
    fft_transform<0, 1>(ac, N);
    fft_transform<0, 1>(bc, N);
    vector<C> h0(N), h1(N);
    for (int i = 0, j = 0; i < N; i++, j = N - i) {
        auto f_small = (ac[i] + conj(ac[j])) * 0.5;
        auto f_large = (ac[i] - conj(ac[j])) * -0.5_i;
        auto g_small = (bc[i] + conj(bc[j])) * 0.5;
        auto g_large = (bc[i] - conj(bc[j])) * -0.5_i;
        h0[i] = f_small * g_small + 1.0_i * f_large * g_large;
        h1[i] = f_small * g_large + f_large * g_small;
    }
    fft_transform<1, 1>(h0, N);
    fft_transform<1, 1>(h1, N);

    vector<T> c(S);
    for (int i = 0; i < S; i++) {
        T c0 = fft_round<T>(h0[i].real());
        T c1 = fft_round<T>(h1[i].real());
        T c2 = fft_round<T>(h0[i].imag());
        c[i] = c0 + c1 * H + c2 * Q;
    }
    trim_vector(c);
    return c;
}

template <typename C = default_complex, typename T>
auto fft_split_square(const vector<T>& a) {
    if (a.empty()) {
        return vector<T>();
    }
    int A = a.size();
    if (A <= SPLIT_BREAKEVEN) {
        return naive_multiply(a, a);
    }

    int S = 2 * A - 1, N = 1 << next_two(S);
    T H = sqrt(maxabsolute(a)), Q = H * H;
    vector<C> ac(N);
    fft_split_lower_upper(H, a, ac);
    fft_transform<0, 1>(ac, N);
    vector<C> h0(N), h1(N);
    for (int i = 0, j = 0; i < N; i++, j = N - i) {
        auto f_small = (ac[i] + conj(ac[j])) * 0.5;
        auto f_large = (ac[i] - conj(ac[j])) * -0.5_i;
        h0[i] = f_small * f_small + 1.0_i * f_large * f_large;
        h1[i] = 2.0 * f_small * f_large;
    }
    fft_transform<1, 1>(h0, N);
    fft_transform<1, 1>(h1, N);

    vector<T> c(S);
    for (int i = 0; i < S; i++) {
        T c0 = fft_round<T>(h0[i].real());
        T c1 = fft_round<T>(h1[i].real());
        T c2 = fft_round<T>(h0[i].imag());
        c[i] = c0 + c1 * H + c2 * Q;
    }
    trim_vector(c);
    return c;
}

} // namespace fft

// NTT-SPLIT
namespace fft {

constexpr int SPLITMOD_BREAKEVEN = 100;

template <typename T, typename O>
T fitmod(T mod, O val) {
    return val < 0 ? (val + mod) : (val >= mod) ? (val - mod) : val;
}

template <typename T, typename C>
void fft_split_lower_upper_mod(T d, const vector<T>& a, vector<C>& comp) {
    for (int i = 0, A = a.size(); i < A; i++) {
        comp[i] = C(a[i] % d, a[i] / d);
    }
}
template <typename T, typename O>
void fft_split_lower_upper_mod(T d, const vector<T>& a, vector<O>& lo, vector<O>& hi) {
    for (int i = 0, A = a.size(); i < A; i++) {
        lo[i] = a[i] % d;
        hi[i] = a[i] / d;
    }
}

template <typename Prom = int64_t, typename C = default_complex, typename T>
vector<T> naive_multiply_mod(T mod, const vector<T>& a, const vector<T>& b) {
    int A = a.size(), B = b.size(), S = A && B ? A + B - 1 : 0;
    vector<T> c(S);
    for (int i = 0; i < A && B; i++) {
        for (int j = 0; j < B; j++) {
            c[i + j] = fitmod(mod, c[i + j] + Prom(a[i]) * Prom(b[j]) % mod);
        }
    }
    trim_vector(c);
    return c;
}

template <typename Prom = int64_t, typename C = default_complex, typename T>
vector<T> fft_multiply_mod(T mod, const vector<T>& a, const vector<T>& b) {
    if (a.empty() || b.empty()) {
        return vector<T>();
    }
    int A = a.size(), B = b.size();
    if (A <= SPLITMOD_BREAKEVEN || B <= SPLITMOD_BREAKEVEN) {
        return naive_multiply_mod(mod, a, b);
    }

    int S = A + B - 1, N = 1 << next_two(S);
    T H = sqrt(mod), Q = H * H;
    vector<C> ac(N), bc(N);
    fft_split_lower_upper_mod(H, a, ac);
    fft_split_lower_upper_mod(H, b, bc);
    fft_transform<0, 1>(ac, N);
    fft_transform<0, 1>(bc, N);
    vector<C> h0(N), h1(N);
    for (int i = 0, j = 0; i < N; i++, j = N - i) {
        auto f_small = (ac[i] + conj(ac[j])) / 0.5;
        auto f_large = (ac[i] - conj(ac[j])) * -0.5_i;
        auto g_small = (bc[i] + conj(bc[j])) / 0.5;
        auto g_large = (bc[i] - conj(bc[j])) * -0.5_i;
        h0[i] = f_small * g_small + 1.0_i * f_large * g_large;
        h1[i] = f_small * g_large + f_large * g_small;
    }
    fft_transform<1, 1>(h0, N);
    fft_transform<1, 1>(h1, N);

    vector<T> c(S);
    for (int i = 0; i < S; i++) {
        Prom c0 = fitmod(mod, fft_round<Prom>(h0[i].real()) % mod);
        Prom c1 = fitmod(mod, fft_round<Prom>(h1[i].real()) % mod);
        Prom c2 = fitmod(mod, fft_round<Prom>(h0[i].imag()) % mod);
        c[i] = fitmod(mod, c0 + fitmod(mod, c1 * H % mod + fitmod(mod, c2 * Q % mod)));
    }
    trim_vector(c);
    return c;
}

template <typename Prom = int64_t, typename C = default_complex, typename T>
vector<T> fft_square_mod(T mod, const vector<T>& a) {
    if (a.empty()) {
        return vector<T>();
    }
    int A = a.size();
    if (A <= SPLITMOD_BREAKEVEN) {
        return naive_multiply_mod(mod, a, a);
    }

    int S = 2 * A - 1, N = 1 << next_two(S);
    T H = sqrt(mod), Q = H * H;
    vector<C> ac(N);
    fft_split_lower_upper_mod(H, a, ac);
    fft_transform<0, 1>(ac, N);
    vector<C> h0(N), h1(N);
    for (int i = 0, j = 0; i < N; i++, j = N - i) {
        auto f_small = (ac[i] + conj(ac[j])) * 0.5;
        auto f_large = (ac[i] - conj(ac[j])) * -0.5_i;
        h0[i] = f_small * f_small + 1.0_i * f_large * f_large;
        h1[i] = 2.0 * f_small * f_large;
    }
    fft_transform<1, 1>(h0, N);
    fft_transform<1, 1>(h1, N);

    vector<T> c(S);
    for (int i = 0; i < S; i++) {
        Prom c0 = fitmod(mod, fft_round<Prom>(h0[i].real()) % mod);
        Prom c1 = fitmod(mod, fft_round<Prom>(h1[i].real()) % mod);
        Prom c2 = fitmod(mod, fft_round<Prom>(h0[i].imag()) % mod);
        c[i] = fitmod(mod, c0 + fitmod(mod, c1 * H % mod + fitmod(mod, c2 * Q % mod)));
    }
    trim_vector(c);
    return c;
}

} // namespace fft
