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

template <typename T, typename O = T>
auto subset_convolution(const vector<T>& f, const vector<T>& g) {
    int N = f.size(), n = N > 1 ? 8 * sizeof(N) - __builtin_clz(N - 1) : 0;
    assert(N == (1 << n) && N == int(g.size()));
    vector<vector<O>> fhat(n + 1, vector<O>(N));
    vector<vector<O>> ghat(n + 1, vector<O>(N));
    for (int mask = 0; mask < N; mask++) {
        int bits = mask ? __builtin_popcount(mask) : 0;
        fhat[bits][mask] = f[mask];
        ghat[bits][mask] = g[mask];
    }
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j < n; j++) {
            for (int bit = 1 << j, mask = 0; mask < N; mask++) {
                if (mask & bit) {
                    fhat[i][mask] += fhat[i][mask ^ bit];
                    ghat[i][mask] += ghat[i][mask ^ bit];
                }
            }
        }
    }
    vector<vector<O>> h(n + 1, vector<O>(N));
    for (int mask = 0; mask < N; mask++) {
        for (int i = 0; i <= n; i++) {
            for (int j = 0; j <= i; j++) {
                h[i][mask] += fhat[j][mask] * ghat[i - j][mask];
            }
        }
    }
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j < n; j++) {
            for (int bit = 1 << j, mask = 0; mask < N; mask++) {
                if (mask & bit) {
                    h[i][mask] -= h[i][mask ^ bit];
                }
            }
        }
    }
    vector<O> S(N);
    for (int mask = 0; mask < N; mask++) {
        int bits = mask ? __builtin_popcount(mask) : 0;
        S[mask] = h[bits][mask];
    }
    return S;
}

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
    auto c = subset_convolution(a, b);
    for (int i = 0; i < N; i++) {
        cout << c[i] << " \n"[i + 1 == N];
    }
    return 0;
}
