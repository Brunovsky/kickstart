#include <bits/stdc++.h>

using namespace std;

// *****

inline namespace {

template <int mod>
struct modnum {
    static_assert(mod > 0 && 2LL * mod < INT_MAX);
    int n;

    modnum() : n(0) {}
    modnum(int v) : n(fit(v % mod)) {}
    explicit operator int() const { return n; }

    static int fit(int v) { return v >= mod ? v - mod : (v < 0 ? v + mod : v); }
    static int modinv(int v, int m = mod) {
        v %= m, assert(v);
        return v == 1 ? 1 : (m - 1LL * modinv(m, v) * m / v);
    }

    modnum inv() const { return {modinv(n)}; }
    modnum operator-() const { return {fit(n)}; }
    modnum operator+() const { return {n}; }
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

    friend bool operator==(modnum lhs, modnum rhs) { return lhs.n == rhs.n; }
    friend bool operator!=(modnum lhs, modnum rhs) { return lhs.n != rhs.n; }
    friend ostream& operator<<(ostream& out, modnum v) { return out << v.n; }
    friend istream& operator>>(istream& in, modnum& v) {
        return in >> v.n, v.n = fit(v.n % mod), in;
    }
};

using num = modnum<1'000'000'007>;

vector<num> fac;

void pascal_sieve(int N) {
    fac.resize(N + 1);
    fac[0] = 1;
    for (int n = 1; n <= N; n++) {
        fac[n] = n * fac[n - 1];
    }
}

num choose(int n, int k) { return fac[n] / (fac[k] * fac[n - k]); }

} // namespace

int N;
vector<int> y;
vector<vector<int>> by_value;

int get_last_in_range(int n, int l, int r) {
    if (by_value[n].empty()) {
        return -1;
    } else {
        auto it = lower_bound(begin(by_value[n]), end(by_value[n]), r);
        if (it == begin(by_value[n]) || *prev(it) < l || *prev(it) >= r) {
            return -1;
        } else {
            return *prev(it);
        }
    }
}

num dp(int n, int l, int r) {
    if (l == r) {
        return 1;
    } else {
        assert(l < r);
        int i = get_last_in_range(n, l, r);
        if (i == -1 || i < l || i >= r) {
            return 0;
        } else {
            return choose(r - l - 1, i - l) * dp(n, l, i) * dp(n + 1, i + 1, r);
        }
    }
}

auto solve() {
    int N;
    cin >> N;
    y.resize(N);
    by_value.assign(N + 1, {});
    for (int i = 0; i < N; i++) {
        cin >> y[i];
        by_value[y[i]].push_back(i);
    }
    return brute();
    // return dp(1, 0, N);
}

// *****

int main() {
    pascal_sieve(200'000);
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
