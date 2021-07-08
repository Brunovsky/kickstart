#include <bits/stdc++.h>
#ifdef LOCAL
#include "code/formatting.hpp"
#else
#define debug(...) (void)0
#endif

using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

template <int A, int B, int C, int D, bool reverse, typename T>
void conv_transform(vector<T> &a) {
    int N = a.size();
    assert((N & (N - 1)) == 0);
    for (int h = 1; h < N; h *= 2) {
        for (int i = 0; i < N; i += 2 * h) {
            for (int j = i; j < i + h; j++) {
                auto x = a[j];
                auto y = a[j + h];
                if constexpr (reverse) {
                    a[j] = D * x - B * y;
                    a[j + h] = -C * x + A * y;
                } else {
                    a[j] = A * x + B * y;
                    a[j + h] = C * x + D * y;
                }
            }
        }
    }
    if constexpr (inverse && A * D - B * C != 0) {
        static_assert(A * D - B * C != 0);
        T div = 1;
        int n = N > 1 ? 8 * sizeof(N) - __builtin_clz(N - 1) : 0;
        while (n--) {
            div *= T(A * D - B * C);
        }
        for (int i = 0; i < N; i++) {
            a[i] /= div;
        }
    }
}

template <int A, int B, int C, int D, typename T>
auto convolve(const vector<T> &a, const vector<T> &b) {
    if (a.empty() || b.empty()) {
        return vector<T>();
    }
    int AS = a.size(), BS = b.size(), M = max(AS, BS);
    int N = 1 << (M > 1 ? 8 * sizeof(M) - __builtin_clz(M - 1) : 0);
    vector<T> c = a, d = b;
    c.resize(N), d.resize(N);
    conv_transform<A, B, C, D, 0>(c);
    conv_transform<A, B, C, D, 0>(d);
    for (int i = 0; i < N; i++) {
        c[i] = c[i] * d[i];
    }
    conv_transform<A, B, C, D, 1>(c);
    return c;
}

template <typename T>
auto xor_convolution(const vector<T> &a, const vector<T> &b) {
    return convolve<1, 1, 1, -1>(a, b);
}

template <typename T>
auto and_convolution(const vector<T> &a, const vector<T> &b) {
    return convolve<0, 1, 1, 1>(a, b);
}

template <typename T>
auto or_convolution(const vector<T> &a, const vector<T> &b) {
    return convolve<1, 1, 1, 0>(a, b);
}

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
    friend constexpr modnum modpow(modnum b, long e) {
        modnum p = 1;
        while (e > 0) {
            if (e & 1)
                p = p * b;
            if (e >>= 1)
                b = b * b;
        }
        return p;
    }

    constexpr modnum inv() const { return modinv(n); }
    constexpr modnum operator-() const { return n == 0 ? n : mod - n; }
    constexpr modnum operator+() const { return *this; }
    constexpr modnum operator++(int) { return n = fit(n + 1), *this - 1; }
    constexpr modnum operator--(int) { return n = fit(mod + n - 1), *this + 1; }
    constexpr modnum &operator++() { return n = fit(n + 1), *this; }
    constexpr modnum &operator--() { return n = fit(mod + n - 1), *this; }
    constexpr modnum &operator+=(modnum v) { return n = fit(n + v.n), *this; }
    constexpr modnum &operator-=(modnum v) { return n = fit(mod + n - v.n), *this; }
    constexpr modnum &operator*=(modnum v) { return n = (u64(n) * v.n) % mod, *this; }
    constexpr modnum &operator/=(modnum v) { return *this *= v.inv(); }

    friend constexpr modnum operator+(modnum lhs, modnum rhs) { return lhs += rhs; }
    friend constexpr modnum operator-(modnum lhs, modnum rhs) { return lhs -= rhs; }
    friend constexpr modnum operator*(modnum lhs, modnum rhs) { return lhs *= rhs; }
    friend constexpr modnum operator/(modnum lhs, modnum rhs) { return lhs /= rhs; }

    friend string to_string(modnum v) { return to_string(v.n); }
    friend constexpr bool operator==(modnum lhs, modnum rhs) { return lhs.n == rhs.n; }
    friend constexpr bool operator!=(modnum lhs, modnum rhs) { return lhs.n != rhs.n; }
    friend ostream &operator<<(ostream &out, modnum v) { return out << v.n; }
    friend istream &operator>>(istream &in, modnum &v) {
        int64_t n;
        return in >> n, v = modnum(n), in;
    }
};

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    using num = modnum<998244353>;
    int n;
    cin >> n;
    int N = 1 << n;
    vector<num> a(N), b(N);
    for (int i = 0; i < N; i++) {
        cin >> a[i];
    }
    for (int i = 0; i < N; i++) {
        cin >> b[i];
    }
    auto c = xor_convolution(a, b);
    for (int i = 0; i < N; i++) {
        cout << c[i] << " \n"[i + 1 == N];
    }
    return 0;
}
