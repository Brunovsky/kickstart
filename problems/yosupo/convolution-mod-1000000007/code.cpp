#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

namespace fft {

int next_two(int32_t N) { return N > 1 ? 8 * sizeof(N) - __builtin_clz(N - 1) : 0; }
int next_two(int64_t N) { return N > 1 ? 8 * sizeof(N) - __builtin_clzll(N - 1) : 0; }

using default_complex = complex<double>;
constexpr double TAU = 6.283185307179586476925286766559;
constexpr int INT4_BREAKEVEN = 1400;
constexpr int INT8_BREAKEVEN = 350;
constexpr int DOUBLE_BREAKEVEN = 650;

inline namespace detail {

template <typename T>
struct root_of_unity {};

template <typename D>
struct root_of_unity<complex<D>> {
    static auto get(int n) {
        assert(n > 0);
        return complex<D>(cos(TAU / n), sin(TAU / n));
    }
};

struct fft_reverse_cache {
    static inline vector<vector<int>> rev;

    static const int* get(int N) {
        int n = next_two(N);
        for (int r = rev.size(); r <= n; r++) {
            int R = 1 << r;
            rev.emplace_back(R, 0);
            for (int i = 0; i < R; i++) {
                rev[r][i] = (rev[r][i >> 1] | ((i & 1) << r)) >> 1;
            }
        }
        return rev[n].data();
    }
};

template <typename C>
struct fft_cache {
    static inline vector<C> root = vector<C>(2, 1);
    static inline vector<C> inv = vector<C>(2, 1);
    static inline vector<C> scratch_a, scratch_b;

    static array<const C*, 2> get_root(int N) {
        for (int k = root.size(); k < N; k *= 2) {
            root.resize(2 * k);
            inv.resize(2 * k);
            auto z = root_of_unity<C>::get(2 * k);
            auto iz = C(1) / z;
            for (int i = k / 2; i < k; i++) {
                root[2 * i] = root[i];
                root[2 * i + 1] = root[i] * z;
                inv[2 * i] = inv[i];
                inv[2 * i + 1] = inv[i] * iz;
            }
        }
        return {root.data(), inv.data()};
    }

    static array<C*, 2> get_cache(int N) {
        if (int(scratch_a.size()) < N) {
            scratch_a.resize(N);
            scratch_b.resize(N);
        }
        return {scratch_a.data(), scratch_b.data()};
    }
};

struct int_ext {
    template <typename C>
    static auto get(const C& c) {
        return llround(c.real());
    }
};

struct real_ext {
    template <typename C>
    static auto get(const C& c) {
        return c.real();
    }
};

struct imag_ext {
    template <typename C>
    static auto get(const C& c) {
        return c.imag();
    }
};

struct exact_ext {
    template <typename C>
    static const C& get(const C& c) {
        return c;
    }
};

template <bool inverse, typename C>
void fft_transform_run(C* a, int N) {
    auto rev = fft_reverse_cache::get(N);
    auto [root, inv] = fft_cache<C>::get_root(N);
    for (int i = 0; i < N; i++) {
        if (i < rev[i]) {
            swap(a[i], a[rev[i]]);
        }
    }
    for (int k = 1; k < N; k *= 2) {
        for (int i = 0; i < N; i += 2 * k) {
            for (int l = i, r = i + k, j = 0; j < k; j++, l++, r++) {
                auto z = inverse ? inv[j + k] : root[j + k];
                auto t = a[r] * z;
                a[r] = a[l] - t;
                a[l] = a[l] + t;
            }
        }
    }
    if constexpr (inverse) {
        auto inverseN = C(1) / C(N);
        for (int i = 0; i < N; i++) {
            a[i] *= inverseN;
        }
    }
}

template <typename Ext, typename C, typename T>
void fft_inverse_transform_run(T* a, C* c, int N) {
    fft_transform_run<1, C>(c, N);
    for (int i = 0; i < N; i++) {
        a[i] = Ext::get(c[i]);
    }
}

template <typename Ext, typename C, typename T, typename OT>
void fft_multiply_run(const T* a, int A, const T* b, int B, OT* c) {
    int S = A + B - 1, N = 1 << next_two(S);
    auto [fa, fb] = fft_cache<C>::get_cache(N);
    copy_n(a, A, fa);
    fill_n(fa + A, N - A, C(0));
    copy_n(b, B, fb);
    fill_n(fb + B, N - B, C(0));
    fft_transform_run<0, C>(fa, N); // forward fft A
    fft_transform_run<0, C>(fb, N); // forward fft B
    for (int i = 0; i < N; i++) {
        fa[i] = fa[i] * fb[i]; // multiply A = A * B
    }
    fft_transform_run<1, C>(fa, N); // reverse fft A
    for (int i = 0; i < S; i++) {
        c[i] = Ext::get(fa[i]);
    }
}

template <typename Ext, typename C, typename T, typename OT>
void fft_square_run(const T* a, int A, OT* c) {
    int S = 2 * A - 1, N = 1 << next_two(S);
    auto [fa, fb] = fft_cache<C>::get_cache(N);
    copy_n(a, A, fa);
    fill_n(fa + A, N - A, C(0));
    fft_transform_run<0, C>(fa, N); // forward fft A
    for (int i = 0; i < N; i++) {
        fa[i] = fa[i] * fa[i]; // multiply A = A * A
    }
    fft_transform_run<1, C>(fa, N); // reverse fft A
    for (int i = 0; i < S; i++) {
        c[i] = Ext::get(fa[i]);
    }
}

template <typename T>
void trim(vector<T>& v) {
    if constexpr (is_floating_point<T>::value)
        while (!v.empty() && abs(v.back()) < 30 * numeric_limits<T>::epsilon())
            v.pop_back();
    else
        while (!v.empty() && v.back() == T(0))
            v.pop_back();
}

template <typename T>
void naive_multiply_run(const T* a, int A, const T* b, int B, T* c) {
    for (int i = 0; i < A && B; i++)
        for (int j = 0; j < B; j++)
            c[i + j] += a[i] * b[j];
}

template <typename T>
void naive_square_run(const T* a, int A, T* c) {
    for (int i = 0; i < A; i++)
        for (int j = 0; j < A; j++)
            c[i + j] += a[i] * a[j];
}

} // namespace detail

template <typename C = default_complex, typename T>
auto fft_multiply(const vector<T>& a, const vector<T>& b) {
    int A = a.size(), B = b.size(), S = A && B ? A + B - 1 : 0;
    vector<T> c(S);
    if (S == 0)
        return c;

    static_assert(is_integral<T>::value || is_floating_point<T>::value);

    if constexpr (is_integral<T>::value) {
        if (sizeof(T) <= 4 && (A <= INT4_BREAKEVEN || B <= INT4_BREAKEVEN)) {
            naive_multiply_run(a.data(), A, b.data(), B, c.data());
        } else if (sizeof(T) > 4 && (A <= INT8_BREAKEVEN || B <= INT8_BREAKEVEN)) {
            naive_multiply_run(a.data(), A, b.data(), B, c.data());
        } else {
            fft_multiply_run<int_ext, C>(a.data(), A, b.data(), B, c.data());
        }
    } else {
        if (A <= DOUBLE_BREAKEVEN || B <= DOUBLE_BREAKEVEN) {
            naive_multiply_run(a.data(), A, b.data(), B, c.data());
        } else {
            fft_multiply_run<real_ext, C>(a.data(), A, b.data(), B, c.data());
        }
    }

    return c;
}

template <typename C = default_complex, typename T>
auto fft_square(const vector<T>& a) {
    int A = a.size(), S = A ? 2 * A - 1 : 0;
    vector<T> c(S);
    if (S == 0)
        return c;

    static_assert(is_integral<T>::value || is_floating_point<T>::value);

    if constexpr (is_integral<T>::value) {
        if (sizeof(T) <= 4 && A <= INT4_BREAKEVEN) {
            naive_square_run(a.data(), A, c.data());
        } else if (sizeof(T) > 4 && A <= INT8_BREAKEVEN) {
            naive_square_run(a.data(), A, c.data());
        } else {
            fft_square_run<int_ext, C>(a.data(), A, c.data());
        }
    } else {
        if (A <= DOUBLE_BREAKEVEN) {
            naive_square_run(a.data(), A, c.data());
        } else {
            fft_square_run<real_ext, C>(a.data(), A, c.data());
        }
    }

    return c;
}

template <typename C = default_complex, typename T>
auto fft_transform(const vector<T>& a) {
    int A = a.size(), n = next_two(A), N = 1 << n;
    vector<C> c(N);
    if (A == 0)
        return c;

    copy_n(a.data(), A, c.data());
    fft_transform_run<0, C>(c.data(), N);
    return c;
}

template <typename T, typename C>
auto fft_inverse_transform(vector<C> c) {
    int N = c.size();
    vector<T> a(N);
    if (N == 0)
        return a;

    if constexpr (is_integral<T>::value) {
        fft_inverse_transform_run<int_ext>(a.data(), c.data(), N);
    } else {
        fft_inverse_transform_run<real_ext>(a.data(), c.data(), N);
    }
    trim(a);
    return a;
}

} // namespace fft

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

int main() {
    ios::sync_with_stdio(false);
    int N, M;
    cin >> N >> M;
    vector<int> a(N), b(M);
    for (int i = 0; i < N; i++)
        cin >> a[i];
    for (int i = 0; i < M; i++)
        cin >> b[i];
    auto c = fft::fft_multiply_mod(1'000'000'007, a, b);
    int C = c.size();
    for (int i = 0; i < C; i++)
        cout << c[i] << " \n"[i + 1 == C];
    return 0;
}
