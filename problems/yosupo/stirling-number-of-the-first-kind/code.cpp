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

constexpr int MODNUM_BREAKEVEN = 0;

template <typename T>
void trim_vector(vector<T>& v) {
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
            rev[n].assign(N, 0);
            for (int i = 0; i < R; i++) {
                rev[n][i] = (rev[n][i >> 1] | ((i & 1) << n)) >> 1;
            }
        }
        return rev[n].data();
    }
};

template <typename T>
struct root_of_unity {};

template <typename C>
struct fft_roots_cache {
    static inline vector<C> root = vector<C>(2, C(1));
    static inline vector<C> invroot = vector<C>(2, C(1));
    static inline vector<C> scratch_a, scratch_b;

    static array<const C*, 2> get(int N) {
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
        return {root.data(), invroot.data()};
    }

    static array<C*, 2> get_cache(int N) {
        if (int(scratch_a.size()) < N) {
            scratch_a.resize(N);
            scratch_b.resize(N);
        }
        return {scratch_a.data(), scratch_b.data()};
    }
};

} // namespace fft

/**
 * NTT with modnums
 */
namespace fft {

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

template <bool inverse, bool reverse, typename T>
void ntt_transform(vector<T>& a) {
    int N = a.size();
    auto rev = fft_reverse_cache::get(N);
    if constexpr (reverse) {
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
        auto inv = N == 1 ? 1 : T(1) / T(N);
        for (int i = 0; i < N; i++) {
            a[i] *= inv;
        }
    }
}

template <typename T>
vector<T> ntt_multiply(const vector<T>& a, const vector<T>& b) {
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
    ntt_transform<0, 1>(c);
    ntt_transform<0, 1>(d);
    for (int i = 0; i < N; i++) {
        c[i] = c[i] * d[i];
    }
    ntt_transform<1, 1>(c);
    trim_vector(c);
    return c;
}

template <typename T>
vector<T> ntt_square(const vector<T>& a) {
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
    ntt_transform<0, 1>(c);
    for (int i = 0; i < N; i++) {
        c[i] = c[i] * c[i];
    }
    ntt_transform<1, 1>(c);
    trim_vector(c);
    return c;
}

} // namespace fft

namespace polymath {

#define tmpl(T) template <typename T>

tmpl(T) auto multiply(const vector<T>& a, const vector<T>& b) {
    return fft::ntt_multiply(a, b);
}

tmpl(T) auto square(const vector<T>& a) { return fft::ntt_square(a); }

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

template <typename T>
auto falling_factorial(int n) {
    vector<T> roots(n);
    iota(begin(roots), end(roots), T(0));
    return withroots(roots);
}

template <typename T>
auto rising_factorial(int n) {
    vector<T> roots(n);
    iota(begin(roots), end(roots), T(1 - n));
    return withroots(roots);
}

/**
 * Compute faul[0..n] such that faul[i] = 1^i + 2^i + ... + m^i
 * Complexity: O(n log n) or O(n^2)
 */
template <typename T>
auto faulhaber(T m, int n) {
    vector<T> ex(n + 1);
    T e = 1;
    for (int i = 0; i <= n; i++) {
        ex[i] = e;
        e /= i + 1;
    }
    vector<T> den = ex;
    den.erase(den.begin());
    for (auto& d : den) {
        d = -d;
    }
    vector<T> num(n);
    T p = 1;
    for (int i = 0; i < n; i++) {
        p *= m + 1;
        num[i] = ex[i + 1] * (1 - p);
    }
    vector<T> faul = truncated(num * inverse_series(den, n), n);
    T f = 1;
    for (int i = 0; i < n; i++) {
        faul[i] *= f;
        f *= i + 1;
    }
    return faul;
}

/**
 * Compute the Nth row of stirling numbers of the first kind.
 * stirling_1st[k] = number of permutations with k disjoint cycles.
 * Complexity: O(n log n) or O(n^2)
 */
template <typename T>
auto stirling_1st(int n) {
    vector<T> roots(n); // same as rising factorial
    iota(begin(roots), end(roots), T(1 - n));
    return withroots(roots);
}

/**
 * Compute the Nth row of the stirling numbers of the second kind.
 * stirling_2nd[k] = number of partitions of n elements into k nonempty disjoint groups.
 * Complexity: O(n log n) or O(n^2)
 */
template <typename T>
auto stirling_2nd(int n) {
    // convolve a[i] = (-1)^i / i!  with  b[i] = i^n / i!
    vector<T> a(n + 1), b(n + 1);
    a[0] = T(1);
    T f = 1;
    for (int i = 1; i <= n; i++) {
        f /= T(i);
        a[i] = (i & 1) ? -f : f;
        b[i] = binpow(i, n) * f;
    }
    return n ? truncated(a * b, n + 1) : vector<T>{T(1)};
}

} // namespace polymath

int main() {
    using namespace polymath;
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int N;
    cin >> N;
    auto p = falling_factorial<modnum<998244353>>(N);
    for (int i = 0; i <= N; i++) {
        cout << p[i] << " \n"[i == N];
    }
    return 0;
}
