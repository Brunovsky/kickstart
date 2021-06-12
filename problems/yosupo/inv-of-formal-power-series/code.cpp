#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

template <int mod>
struct modnum {
    static_assert(mod > 0 && 2LL * mod < INT_MAX);
    int n;

    modnum() : n(0) {}
    modnum(int v) : n(fit(v % mod)) {}
    explicit operator int() const { return n; }
    explicit operator bool() const { return n != 0; }

    static int fit(int v) { return v >= mod ? v - mod : (v < 0 ? v + mod : v); }
    static int modinv(int v, int m = mod) {
        v %= m, assert(v);
        return v == 1 ? 1 : (m - 1LL * modinv(m, v) * m / v);
    }
    friend modnum modpow(modnum b, long e) {
        modnum p = 1;
        while (e > 0) {
            if (e & 1)
                p = p * b;
            if (e >>= 1)
                b = b * b;
        }
        return p;
    }

    modnum inv() const { return {modinv(n)}; }
    modnum operator-() const { return {fit(-n)}; }
    modnum operator+() const { return {n}; }
    modnum operator++(int) { return n = fit(n + 1), *this - 1; }
    modnum operator--(int) { return n = fit(n - 1), *this + 1; }
    modnum& operator++() { return n = fit(n + 1), *this; }
    modnum& operator--() { return n = fit(n - 1), *this; }
    modnum& operator+=(modnum v) { return n = fit(n + v.n), *this; }
    modnum& operator-=(modnum v) { return n = fit(n - v.n), *this; }
    modnum& operator*=(modnum v) { return n = (1LL * n * v.n) % mod, *this; }
    modnum& operator/=(modnum v) { return n = (1LL * n * modinv(v.n)) % mod, *this; }

    friend modnum operator+(modnum lhs, modnum rhs) { return lhs += rhs; }
    friend modnum operator-(modnum lhs, modnum rhs) { return lhs -= rhs; }
    friend modnum operator*(modnum lhs, modnum rhs) { return lhs *= rhs; }
    friend modnum operator/(modnum lhs, modnum rhs) { return lhs /= rhs; }

    friend string to_string(modnum v) { return to_string(v.n); }
    friend bool operator==(modnum lhs, modnum rhs) { return lhs.n == rhs.n; }
    friend bool operator!=(modnum lhs, modnum rhs) { return lhs.n != rhs.n; }
    friend ostream& operator<<(ostream& out, modnum v) { return out << v.n; }
    friend istream& operator>>(istream& in, modnum& v) {
        return in >> v.n, v.n = fit(v.n % mod), in;
    }
};

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

namespace polymath {

#define tmpl(T) template <typename T>

tmpl(T) auto multiply(const vector<T>& a, const vector<T>& b) {
    return fft::fft_multiply(a, b);
}

tmpl(T) auto square(const vector<T>& a) { return fft::fft_square(a); }

tmpl(T) T binpow(T val, long e) {
    T base = {1};
    while (e > 0) {
        if (e & 1)
            base *= val;
        if (e >>= 1)
            val *= val;
    }
    return base;
}

tmpl(T) void trim(vector<T>& a) {
    if constexpr (is_floating_point<T>::value)
        while (!a.empty() && abs(a.back()) < 30 * numeric_limits<T>::epsilon())
            a.pop_back();
    else
        while (!a.empty() && a.back() == T())
            a.pop_back();
}

tmpl(T) void truncate(vector<T>& v, int size) { v.resize(min(int(v.size()), size)); }

tmpl(T) auto truncated(vector<T> v, int size) { return truncate(v, size), v; }

tmpl(T) auto eval(const vector<T>& a, T x) {
    T v = 0;
    for (int A = a.size(), i = A - 1; i >= 0; i--)
        v = a[i] + v * x;
    return v;
}

tmpl(T) auto convolve(const vector<T>& a, vector<T> b) {
    reverse(begin(b), end(b));
    return a * b;
}

tmpl(T) auto deriv(vector<T> a) {
    int N = a.size();
    for (int i = 0; i + 1 < N; i++)
        a[i] = T(i + 1) * a[i + 1];
    if (N > 0)
        a.pop_back();
    return a;
}

tmpl(T) auto integr(vector<T> a, T c = T()) {
    int N = a.size();
    a.resize(N + 1);
    for (int i = N; i > 0; i--)
        a[i] = a[i - 1] / T(i);
    a[0] = c;
    return a;
}

tmpl(T) auto withroots(const vector<T>& roots) {
    int R = roots.size();
    vector<vector<T>> polys(R);

    for (int i = 0; i < R; i++) {
        polys[i] = {-roots[i], T(1)};
    }

    while (R > 1) {
        for (int i = 0; i < R / 2; i++) {
            polys[i] = polys[i << 1] * polys[i << 1 | 1];
        }
        if (R & 1) {
            polys[R / 2] = move(polys[R - 1]);
        }
        R = (R + 1) / 2;
        polys.resize(R);
    }

    return R ? polys[0] : vector<T>{T(1)};
}

tmpl(T) auto& operator*=(vector<T>& a, const vector<T>& b) { return a = multiply(a, b); }

tmpl(T) auto operator*(const vector<T>& a, const vector<T>& b) { return multiply(a, b); }

tmpl(T) auto operator-(vector<T> a) {
    for (int A = a.size(), i = 0; i < A; i++)
        a[i] = -a[i];
    return a;
}

tmpl(T) auto& operator+=(vector<T>& a, const vector<T>& b) {
    int A = a.size(), B = b.size();
    a.resize(max(A, B));
    for (int i = 0; i < B; i++)
        a[i] += b[i];
    trim(a);
    return a;
}

tmpl(T) auto operator+(vector<T> a, const vector<T>& b) { return a += b; }

tmpl(T) auto& operator-=(vector<T>& a, const vector<T>& b) {
    int A = a.size(), B = b.size();
    a.resize(max(A, B));
    for (int i = 0; i < B; i++)
        a[i] -= b[i];
    trim(a);
    return a;
}

tmpl(T) auto operator-(vector<T> a, const vector<T>& b) { return a -= b; }

tmpl(T) auto& operator*=(vector<T>& a, T constant) {
    for (int i = 0, A = a.size(); i < A; i++)
        a[i] *= constant;
    return a;
}

tmpl(T) auto operator*(T constant, vector<T> a) { return a *= constant; }

tmpl(T) auto& operator/=(vector<T>& a, T constant) {
    for (int i = 0, A = a.size(); i < A; i++)
        a[i] /= constant;
    return a;
}

tmpl(T) auto operator/(vector<T> a, T constant) { return a /= constant; }

tmpl(T) auto inverse_series(const vector<T>& a, int mod_degree) {
    assert(!a.empty() && a[0]);
    vector<T> b(1, T(1) / a[0]);

    for (int len = 1; len < mod_degree; len *= 2) {
        b += b - truncated(a, 2 * len) * square(b);
        truncate(b, min(2 * len, mod_degree)), trim(b);
    }

    return b;
}

tmpl(T) auto operator/(vector<T> a, vector<T> b) {
    int A = a.size(), B = b.size();
    if (B > A)
        return vector<T>();

    reverse(begin(a), end(a));
    reverse(begin(b), end(b));
    auto d = a * inverse_series(b, A - B + 1);
    truncate(d, A - B + 1);
    reverse(begin(d), end(d));
    return d;
}

tmpl(T) auto& operator/=(vector<T>& a, const vector<T>& b) { return a = a / b; }

tmpl(T) auto operator%(const vector<T>& a, const vector<T>& b) { return a - b * (a / b); }

tmpl(T) auto& operator%=(vector<T>& a, const vector<T>& b) { return a = a % b; }

tmpl(T) auto division_with_remainder(const vector<T>& a, const vector<T>& b) {
    auto d = a / b, r = a - b * d;
    return make_pair(move(d), move(r));
}

tmpl(T) auto gcd(const vector<T>& a, const vector<T>& b) -> vector<T> {
    return b.empty() ? a.empty() ? a : a / a.back() : gcd(b, a % b);
}

tmpl(T) auto resultant(const vector<T>& a, const vector<T>& b) {
    int A = a.size(), B = b.size();
    if (B == 0) {
        return T();
    } else if (B == 1) {
        return binpow(b[0], A - 1);
    } else {
        auto c = a % b;
        A -= c.size();
        auto mul = binpow(b[0], A - 1) * T(((A - 1) & (B - 1) & 1) ? -1 : 1);
        return mul * resultant(b, c);
    }
}

#undef tmpl

} // namespace polymath

namespace polymath {

#define tmpl(T) template <typename T>

tmpl(T) struct multieval_tree {
    vector<int> index;
    vector<vector<T>> tree;
    vector<T> x;
};

tmpl(T) auto build_multieval_tree(const vector<T>& x) {
    int N = x.size(), M = 1 << fft::next_two(N);
    vector<int> index(N);
    vector<vector<T>> tree(2 * N);
    for (int i = 0; i < N; i++) {
        index[i] = i;
        tree[i + N] = {-x[i], 1};
    }

    rotate(begin(index), begin(index) + (2 * N - M), end(index));
    rotate(begin(tree) + N, begin(tree) + (3 * N - M), end(tree));

    for (int i = N - 1; i >= 1; i--) {
        int l = i << 1, r = i << 1 | 1;
        tree[i] = tree[l] * tree[r];
    }

    return multieval_tree<T>{move(index), move(tree), x};
}

tmpl(T) void multieval_dfs(int i, const vector<T>& poly, vector<T>& value,
                           const multieval_tree<T>& evaltree) {
    const auto& [index, tree, x] = evaltree;
    if (int N = x.size(); i >= N) {
        int j = index[i - N];
        value[j] = eval(poly, x[j]);
    } else {
        int l = i << 1, r = i << 1 | 1;
        multieval_dfs(l, poly % tree[l], value, evaltree);
        multieval_dfs(r, poly % tree[r], value, evaltree);
    }
}

tmpl(T) auto multieval(const vector<T>& poly, const multieval_tree<T>& evaltree) {
    vector<T> value(evaltree.x.size());
    multieval_dfs(1, poly % evaltree.tree[1], value, evaltree);
    return value;
}

tmpl(T) auto multieval(const vector<T>& poly, const vector<T>& x) {
    return multieval(poly, build_multieval_tree(x));
}

tmpl(T) auto interpolate_dfs(int i, const vector<T>& poly, const vector<T>& y,
                             const multieval_tree<T>& evaltree) {
    const auto& [index, tree, x] = evaltree;
    if (int N = x.size(); i >= N) {
        int j = index[i - N];
        return vector<T>{y[j] / poly[0]};
    } else {
        int l = i << 1, r = i << 1 | 1;
        auto a = interpolate_dfs(l, poly % tree[l], y, evaltree);
        auto b = interpolate_dfs(r, poly % tree[r], y, evaltree);
        return a * tree[r] + b * tree[l];
    }
}

tmpl(T) auto interpolate(const vector<T>& x, const vector<T>& y) {
    assert(x.size() == y.size());
    auto evaltree = build_multieval_tree(x);
    return interpolate_dfs(1, deriv(evaltree.tree[1]), y, evaltree);
}

#undef tmpl

} // namespace polymath

int main() {
    using namespace polymath;
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int N;
    cin >> N;
    using num = modnum<998244353>;
    vector<num> f(N);
    for (int i = 0; i < N; i++) {
        cin >> f[i];
    }
    auto g = inverse_series(f, N);
    for (int i = 0; i < N; i++) {
        cout << g[i] << " \n"[i + 1 == N];
    }
    return 0;
}
