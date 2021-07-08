#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

template <uint32_t mod>
struct modnum {
    using u32 = uint32_t;
    using u64 = uint64_t;
    static_assert(mod > 0 && mod < UINT_MAX / 2);
    uint32_t n;

    constexpr modnum() : n(0) {}
    constexpr modnum(u64 v) : n(v >= mod ? v % mod : v) {}
    constexpr modnum(u32 v) : n(v >= mod ? v % mod : v) {}
    constexpr modnum(int64_t v) : modnum(v >= 0 ? u64(v) : u64(mod + v % int(mod))) {}
    constexpr modnum(int32_t v) : modnum(v >= 0 ? u32(v) : u32(mod + v % int(mod))) {}
    explicit constexpr operator int() const { return n; }
    explicit constexpr operator bool() const { return n != 0; }

    static constexpr u32 fit(u32 x) { return x >= mod ? x - mod : x; }
    static constexpr int modinv(u32 x) {
        int nx = 1, ny = 0;
        u32 y = mod;
        while (x) {
            auto k = y / x;
            y = y % x;
            ny = ny - k * nx;
            swap(x, y), swap(nx, ny);
        }
        return ny < 0 ? mod + ny : ny;
    }
    static constexpr modnum modpow(modnum b, long e) {
        modnum p = 1;
        while (e > 0) {
            if (e & 1)
                p = p * b;
            if (e >>= 1)
                b = b * b;
        }
        return p;
    }

    constexpr modnum inv() const { return modpow(*this, mod - 2); }
    constexpr modnum operator-() const { return n == 0 ? n : mod - n; }
    constexpr modnum operator+() const { return *this; }
    constexpr modnum operator++(int) { return n = fit(n + 1), *this - 1; }
    constexpr modnum operator--(int) { return n = fit(mod + n - 1), *this + 1; }
    constexpr modnum& operator++() { return n = fit(n + 1), *this; }
    constexpr modnum& operator--() { return n = fit(mod + n - 1), *this; }
    constexpr modnum& operator+=(modnum v) { return n = fit(n + v.n), *this; }
    constexpr modnum& operator-=(modnum v) { return n = fit(mod + n - v.n), *this; }
    constexpr modnum& operator*=(modnum v) { return n = (u64(n) * v.n) % mod, *this; }
    constexpr modnum& operator/=(modnum v) {
        return n = (u64(n) * modinv(v.n)) % mod, *this;
    }

    friend constexpr modnum operator+(modnum lhs, modnum rhs) { return lhs += rhs; }
    friend constexpr modnum operator-(modnum lhs, modnum rhs) { return lhs -= rhs; }
    friend constexpr modnum operator*(modnum lhs, modnum rhs) { return lhs *= rhs; }
    friend constexpr modnum operator/(modnum lhs, modnum rhs) { return lhs /= rhs; }

    friend string to_string(modnum v) { return to_string(v.n); }
    friend constexpr bool operator==(modnum lhs, modnum rhs) { return lhs.n == rhs.n; }
    friend constexpr bool operator!=(modnum lhs, modnum rhs) { return lhs.n != rhs.n; }
    friend ostream& operator<<(ostream& out, modnum v) { return out << v.n; }
    friend istream& operator>>(istream& in, modnum& v) {
        int64_t n;
        return in >> n, v = modnum(n), in;
    }
};

/**
 * Source: Nyann on yosupo
 */
template <uint32_t mod>
struct montg {
    using u32 = uint32_t;
    using u64 = uint64_t;
    static_assert(mod > 0 && 2LL * mod < INT_MAX);
    static constexpr u32 get_r() {
        u32 ret = mod;
        for (int i = 0; i < 4; ++i)
            ret *= 2 - mod * ret;
        return ret;
    }
    static constexpr u32 r = get_r();
    static constexpr u32 n2 = -u64(mod) % mod;

    u32 a;

    constexpr montg() : a(0) {}
    constexpr montg(const int64_t& b) : a(reduce(u64(b % mod + mod) * n2)) {}
    explicit operator int() const { return a; }
    explicit operator bool() const { return a != 0; }

    static constexpr u32 reduce(const u64& b) {
        return (b + u64(u32(b) * u32(-r)) * mod) >> 32;
    }
    static constexpr montg modpow(montg v, u64 n) {
        montg ret(1);
        while (n > 0) {
            if (n & 1)
                ret *= v;
            if (n >>= 1)
                v *= v;
        }
        return ret;
    }

    constexpr montg inv() const { return modpow(*this, mod - 2); }
    constexpr montg& operator+=(const montg& b) {
        if (int(a += b.a - 2 * mod) < 0)
            a += 2 * mod;
        return *this;
    }
    constexpr montg& operator-=(const montg& b) {
        if (int(a -= b.a) < 0)
            a += 2 * mod;
        return *this;
    }
    constexpr montg& operator*=(const montg& b) {
        return a = reduce(u64(a) * b.a), *this;
    }
    constexpr montg& operator/=(const montg& b) { return *this *= b.inv(); }
    constexpr montg operator+(const montg& b) const { return montg(*this) += b; }
    constexpr montg operator-(const montg& b) const { return montg(*this) -= b; }
    constexpr montg operator*(const montg& b) const { return montg(*this) *= b; }
    constexpr montg operator/(const montg& b) const { return montg(*this) /= b; }
    constexpr bool operator==(const montg& b) const {
        return (a >= mod ? a - mod : a) == (b.a >= mod ? b.a - mod : b.a);
    }
    constexpr bool operator!=(const montg& b) const {
        return (a >= mod ? a - mod : a) != (b.a >= mod ? b.a - mod : b.a);
    }
    constexpr montg operator-() const { return montg() - montg(*this); }

    friend ostream& operator<<(ostream& out, const montg& b) { return out << b.get(); }
    friend istream& operator>>(istream& in, montg& b) {
        int64_t t;
        in >> t;
        b = montg(t);
        return in;
    }

    constexpr u32 get() const {
        u32 ret = reduce(a);
        return ret >= mod ? ret - mod : ret;
    }
};

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
                rev[n][i] = (rev[n][i >> 1] | ((i & 1) << n)) >> 1;
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

template <bool inverse, bool reverse = true, typename T>
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

namespace fft {

constexpr int MODNUM_BREAKEVEN = 150;

int ntt_primitive_root(int p) {
    static unordered_map<int, int> cache = {{998244353, 3}};
    if (cache.count(p)) {
        return cache.at(p);
    }
    assert(false && "Sorry, unimplemented");
}

template <uint32_t MOD>
struct root_of_unity<modnum<MOD>> {
    using type = modnum<MOD>;
    static type get(int n) {
        modnum<MOD> g = ntt_primitive_root(MOD);
        assert(n > 0 && (MOD - 1) % n == 0 && "Modulus cannot handle NTT this large");
        return modnum<MOD>::modpow(g, (MOD - 1) / n);
    }
};

template <uint32_t MOD>
struct root_of_unity<montg<MOD>> {
    using type = montg<MOD>;
    static type get(int n) {
        montg<MOD> g = ntt_primitive_root(MOD);
        assert(n > 0 && (MOD - 1) % n == 0 && "Modulus cannot handle NTT this large");
        return montg<MOD>::modpow(g, (MOD - 1) / n);
    }
};

template <uint32_t MOD>
auto ntt_multiply(const vector<modnum<MOD>>& a, const vector<modnum<MOD>>& b) {
    using T = modnum<MOD>;
    if (a.empty() || b.empty()) {
        return vector<T>();
    }
    int A = a.size(), B = b.size();
    if (A <= MODNUM_BREAKEVEN && B <= MODNUM_BREAKEVEN) {
        return naive_multiply(a, b);
    }

    int C = A + B - 1, N = 1 << next_two(C);
    vector<T> c = a, d = b;
    c.resize(N, T(0));
    d.resize(N, T(0));
    fft_transform<0>(c, N);
    fft_transform<0>(d, N);
    for (int i = 0; i < N; i++) {
        c[i] = c[i] * d[i];
    }
    fft_transform<1>(c, N);
    trim_vector(c);
    return c;
}

template <uint32_t MOD>
auto ntt_multiply(const vector<montg<MOD>>& a, const vector<montg<MOD>>& b) {
    using T = montg<MOD>;
    if (a.empty() || b.empty()) {
        return vector<T>();
    }
    int A = a.size(), B = b.size();
    if (A <= MODNUM_BREAKEVEN && B <= MODNUM_BREAKEVEN) {
        return naive_multiply(a, b);
    }

    int C = A + B - 1, N = 1 << next_two(C);
    vector<T> c = a, d = b;
    c.resize(N, T(0));
    d.resize(N, T(0));
    fft_transform<0>(c, N);
    fft_transform<0>(d, N);
    for (int i = 0; i < N; i++) {
        c[i] = c[i] * d[i];
    }
    fft_transform<1>(c, N);
    trim_vector(c);
    return c;
}

} // namespace fft

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    using num = modnum<998244353>;
    int N, M;
    cin >> N >> M;
    vector<num> a(N), b(M);
    for (int i = 0; i < N; i++) {
        cin >> a[i];
    }
    for (int i = 0; i < M; i++) {
        cin >> b[i];
    }
    auto c = fft::ntt_multiply(a, b);
    c.resize(N + M - 1);
    for (int i = 0; i < N + M - 1; i++) {
        cout << c[i] << " \n"[i + 1 == N + M - 1];
    }
    return 0;
}
