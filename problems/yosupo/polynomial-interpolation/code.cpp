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

// Arbitrary modulus FFT for modnums
namespace fft {

int SPLITMODNUM_BREAKEVEN = 80;

template <typename C, int MOD, typename T = int>
auto fft_split_lower_upper_mod(T H, const vector<modnum<MOD>>& a, vector<C>& comp) {
    for (int i = 0, A = a.size(); i < A; i++) {
        comp[i] = C(T(a[i]) % H, T(a[i]) / H);
    }
}

template <typename C = default_complex, int MOD>
auto fft_multiply(const vector<modnum<MOD>>& a, const vector<modnum<MOD>>& b) {
    using T = modnum<MOD>;
    if (a.empty() || b.empty()) {
        return vector<T>();
    }
    int A = a.size(), B = b.size();
    if (A <= SPLITMODNUM_BREAKEVEN || B <= SPLITMODNUM_BREAKEVEN) {
        return naive_multiply(a, b);
    }

    int S = A + B - 1, N = 1 << next_two(S);
    int H = sqrt(MOD), Q = H * H;
    vector<C> ac(N), bc(N);
    fft_split_lower_upper_mod(H, a, ac);
    fft_split_lower_upper_mod(H, b, bc);
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
        T c0 = fft_round<int64_t>(h0[i].real()) % MOD;
        T c1 = fft_round<int64_t>(h1[i].real()) % MOD;
        T c2 = fft_round<int64_t>(h0[i].imag()) % MOD;
        c[i] = c0 + c1 * H + c2 * Q;
    }
    trim_vector(c);
    return c;
}

template <typename C = default_complex, int MOD>
auto fft_square(const vector<modnum<MOD>>& a) {
    using T = modnum<MOD>;
    if (a.empty()) {
        return vector<T>();
    }
    int A = a.size();
    if (A <= SPLITMODNUM_BREAKEVEN) {
        return naive_multiply(a, a);
    }

    int S = 2 * A - 1, N = 1 << next_two(S);
    int H = sqrt(MOD), Q = H * H;
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
        T c0 = fft_round<int64_t>(h0[i].real()) % MOD;
        T c1 = fft_round<int64_t>(h1[i].real()) % MOD;
        T c2 = fft_round<int64_t>(h0[i].imag()) % MOD;
        c[i] = c0 + c1 * H + c2 * Q;
    }
    trim_vector(c);
    return c;
}

} // namespace fft

namespace polymath {

template <typename T>
auto multiply(const vector<T>& a, const vector<T>& b) {
    return fft::fft_multiply(a, b);
}

template <typename T>
auto square(const vector<T>& a) {
    return fft::fft_square(a);
}

template <typename T>
T binpow(T val, long e) {
    T base = {1};
    while (e > 0) {
        if (e & 1)
            base *= val;
        if (e >>= 1)
            val *= val;
    }
    return base;
}

template <typename T>
void trim(vector<T>& a) {
    if constexpr (is_floating_point<T>::value)
        while (!a.empty() && abs(a.back()) < 30 * numeric_limits<T>::epsilon())
            a.pop_back();
    else
        while (!a.empty() && a.back() == T())
            a.pop_back();
}

template <typename T>
void truncate(vector<T>& v, int size) {
    v.resize(min(int(v.size()), size));
}

template <typename T>
auto truncated(vector<T> v, int size) {
    return truncate(v, size), v;
}

template <typename T>
auto eval(const vector<T>& a, T x) {
    T v = 0;
    for (int A = a.size(), i = A - 1; i >= 0; i--)
        v = a[i] + v * x;
    return v;
}

template <typename T>
auto deriv(vector<T> a) {
    int N = a.size();
    for (int i = 0; i + 1 < N; i++)
        a[i] = T(i + 1) * a[i + 1];
    if (N > 0)
        a.pop_back();
    return a;
}

template <typename T>
auto integr(vector<T> a, T c = T()) {
    int N = a.size();
    a.resize(N + 1);
    for (int i = N; i > 0; i--)
        a[i] = a[i - 1] / T(i);
    a[0] = c;
    return a;
}

template <typename T>
auto withroots(const vector<T>& roots) {
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

template <typename T>
auto& operator*=(vector<T>& a, const vector<T>& b) {
    return a = multiply(a, b);
}

template <typename T>
auto operator*(const vector<T>& a, const vector<T>& b) {
    return multiply(a, b);
}

template <typename T>
auto operator-(vector<T> a) {
    for (int A = a.size(), i = 0; i < A; i++)
        a[i] = -a[i];
    return a;
}

template <typename T>
auto& operator+=(vector<T>& a, const vector<T>& b) {
    int A = a.size(), B = b.size();
    a.resize(max(A, B));
    for (int i = 0; i < B; i++)
        a[i] += b[i];
    trim(a);
    return a;
}

template <typename T>
auto operator+(vector<T> a, const vector<T>& b) {
    return a += b;
}

template <typename T>
auto& operator-=(vector<T>& a, const vector<T>& b) {
    int A = a.size(), B = b.size();
    a.resize(max(A, B));
    for (int i = 0; i < B; i++)
        a[i] -= b[i];
    trim(a);
    return a;
}

template <typename T>
auto operator-(vector<T> a, const vector<T>& b) {
    return a -= b;
}

template <typename T>
auto& operator*=(vector<T>& a, T constant) {
    for (int i = 0, A = a.size(); i < A; i++)
        a[i] *= constant;
    return a;
}

template <typename T>
auto operator*(T constant, vector<T> a) {
    return a *= constant;
}

template <typename T>
auto& operator/=(vector<T>& a, T constant) {
    for (int i = 0, A = a.size(); i < A; i++)
        a[i] /= constant;
    return a;
}

template <typename T>
auto operator/(vector<T> a, T constant) {
    return a /= constant;
}

template <typename T>
auto inverse_series(const vector<T>& a, int mod_degree) {
    assert(!a.empty() && a[0]);
    vector<T> b(1, T(1) / a[0]);

    for (int len = 1; len < mod_degree; len *= 2) {
        b += b - truncated(a, 2 * len) * square(b);
        truncate(b, min(2 * len, mod_degree)), trim(b);
    }

    return b;
}

template <typename T>
auto operator/(vector<T> a, vector<T> b) {
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

template <typename T>
auto& operator/=(vector<T>& a, const vector<T>& b) {
    return a = a / b;
}

template <typename T>
auto operator%(const vector<T>& a, const vector<T>& b) {
    return a - b * (a / b);
}

template <typename T>
auto& operator%=(vector<T>& a, const vector<T>& b) {
    return a = a % b;
}

template <typename T>
auto division_with_remainder(const vector<T>& a, const vector<T>& b) {
    auto d = a / b, r = a - b * d;
    return make_pair(move(d), move(r));
}

template <typename T>
auto gcd(const vector<T>& a, const vector<T>& b) -> vector<T> {
    return b.empty() ? a.empty() ? a : a / a.back() : gcd(b, a % b);
}

template <typename T>
auto resultant(const vector<T>& a, const vector<T>& b) {
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

} // namespace polymath

namespace polymath {

template <typename T>
struct multieval_tree {
    vector<int> index;
    vector<vector<T>> tree;
    vector<T> x;
};

template <typename T>
auto build_multieval_tree(const vector<T>& x) {
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

template <typename T>
void multieval_dfs(int i, const vector<T>& poly, vector<T>& value,
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

template <typename T>
auto multieval(const vector<T>& poly, const multieval_tree<T>& evaltree) {
    vector<T> value(evaltree.x.size());
    multieval_dfs(1, poly % evaltree.tree[1], value, evaltree);
    return value;
}

template <typename T>
auto multieval(const vector<T>& poly, const vector<T>& x) {
    return multieval(poly, build_multieval_tree(x));
}

template <typename T>
auto interpolate_dfs(int i, const vector<T>& poly, const vector<T>& y,
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

template <typename T>
auto interpolate(const vector<T>& x, const vector<T>& y) {
    assert(x.size() == y.size());
    auto evaltree = build_multieval_tree(x);
    return interpolate_dfs(1, deriv(evaltree.tree[1]), y, evaltree);
}

} // namespace polymath

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    using num = modnum<998244353>;
    int N;
    cin >> N;
    vector<num> x(N), y(N);
    for (int i = 0; i < N; i++)
        cin >> x[i];
    for (int i = 0; i < N; i++)
        cin >> y[i];
    auto c = polymath::interpolate(x, y);
    c.resize(N, 0);
    for (int i = 0; i < N; i++) {
        cout << c[i] << " \n"[i + 1 == N];
    }
    return 0;
}
