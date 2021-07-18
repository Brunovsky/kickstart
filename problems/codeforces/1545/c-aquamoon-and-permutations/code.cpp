#include <bits/stdc++.h>
#ifdef LOCAL
#include "code/formatting.hpp"
#else
#define debug(...) (void)0
#endif

using namespace std;
#define long int64_t
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
    constexpr modnum& operator++() { return n = fit(n + 1), *this; }
    constexpr modnum& operator--() { return n = fit(mod + n - 1), *this; }
    constexpr modnum& operator+=(modnum v) { return n = fit(n + v.n), *this; }
    constexpr modnum& operator-=(modnum v) { return n = fit(mod + n - v.n), *this; }
    constexpr modnum& operator*=(modnum v) { return n = (u64(n) * v.n) % mod, *this; }
    constexpr modnum& operator/=(modnum v) { return *this *= v.inv(); }

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

using num = modnum<998244353>;
vector<num> fac, ifac;

auto pascal_sieve(int N) {
    fac.resize(N + 1), ifac.resize(N + 1);
    fac[0] = fac[1] = ifac[1] = 1;

    for (int n = 1; n <= N; n++) {
        fac[n] = n * fac[n - 1];
    }
    ifac[N] = fac[N].inv();
    for (int n = N; n >= 1; n--) {
        ifac[n - 1] = n * ifac[n];
    }
}

num choose(int n, int k) { return k >= 0 && k <= n ? fac[n] * ifac[k] * ifac[n - k] : 0; }

template <typename Fun>
class y_combinator_result {
    Fun fun_;

  public:
    template <typename T>
    explicit y_combinator_result(T&& fun) : fun_(std::forward<T>(fun)) {}

    template <typename... Args>
    decltype(auto) operator()(Args&&... args) {
        return fun_(std::ref(*this), std::forward<Args>(args)...);
    }
};

template <typename Fun>
auto y_combinator(Fun&& fun) {
    return y_combinator_result<std::decay_t<Fun>>(std::forward<Fun>(fun));
}

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int T;
    cin >> T;
    while (T--) {
        int N;
        cin >> N;
        vector<vector<int>> arrs(2 * N, vector<int>(N));
        for (int i = 0; i < 2 * N; i++) {
            for (int k = 0; k < N; k++) {
                cin >> arrs[i][k], arrs[i][k]--;
            }
        }

        vector<vector<vector<int>>> by_pos_val(N, vector<vector<int>>(N));
        for (int i = 0; i < 2 * N; i++) {
            for (int k = 0; k < N; k++) {
                by_pos_val[k][arrs[i][k]].push_back(i);
            }
        }

        vector<vector<bool>> compatible(2 * N, vector<bool>(2 * N, true));
        for (int i = 0; i < 2 * N; i++) {
            for (int j = i; j < 2 * N; j++) {
                for (int k = 0; k < N; k++) {
                    if (arrs[i][k] == arrs[j][k]) {
                        compatible[i][j] = compatible[j][i] = false;
                        break;
                    }
                }
            }
        }

        vector<int> must;
        for (int i = 0; i < N; i++) {
            for (int k = 0; k < N; k++) {
                assert(!by_pos_val[i][k].empty());
                if (by_pos_val[i][k].size() == 1u) {
                    must.push_back(by_pos_val[i][k][0]);
                }
            }
        }

        sort(begin(must), end(must));
        must.erase(unique(begin(must), end(must)), end(must));

        debug(must);

        vector<bool> active(2 * N, true);
        for (int i = 0; i < 2 * N; i++) {
            for (int j : must) {
                if (!compatible[i][j]) {
                    active[i] = false;
                }
            }
        }
        for (int j : must) {
            assert(!active[j]);
        }

        vector<vector<bool>> seen(N, vector<bool>(N));
        for (int j : must) {
            for (int i = 0; i < N; i++) {
                seen[i][arrs[j][i]] = true;
            }
        }

        int ans = 0;
        vector<int> sample;

        mt19937 mt(random_device{}());
        uniform_int_distribution disti(0, N - 1);

        y_combinator([&](auto self) -> void {
            if (int(must.size()) == N) {
                ans++;
                if (ans == 1) {
                    sample = must;
                }
                return;
            }
            int p = disti(mt);
            int n = 0;
            while (seen[p][n]) {
                n++;
            }
            for (int j : by_pos_val[p][n]) {
                if (active[j]) {
                    vector<int> bad;
                    for (int z = 0; z < 2 * N; z++) {
                        if (active[z] && !compatible[j][z]) {
                            bad.push_back(z);
                            active[z] = false;
                        }
                    }
                    for (int k = 0; k < N; k++) {
                        assert(!seen[k][arrs[j][k]]);
                        seen[k][arrs[j][k]] = true;
                    }

                    must.push_back(j);
                    self();
                    must.pop_back();

                    for (int k = 0; k < N; k++) {
                        assert(seen[k][arrs[j][k]]);
                        seen[k][arrs[j][k]] = false;
                    }
                    for (int z : bad) {
                        active[z] = true;
                    }
                }
            }
        })();

        sort(begin(sample), end(sample));

        cout << ans << endl;
        for (int i = 0; i < N; i++) {
            cout << (sample[i] + 1) << " \n"[i + 1 == N];
        }
    }
    return 0;
}
