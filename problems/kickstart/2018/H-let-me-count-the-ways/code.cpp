#include <bits/stdc++.h>

using namespace std;

// *****

constexpr long MOD = 1'000'000'007;
constexpr int MAX = 200'001;

long gcd(long a, long b, long &x, long &y) {
    long xn = 1, yn = 0;
    x = 0, y = 1;
    while (a != 0) {
        long q = b / a;
        b = b % a;
        x = x - q * xn;
        y = y - q * yn;
        swap(a, b);
        swap(x, xn);
        swap(y, yn);
    }
    return b;
}

long invmod(long a, long mod) {
    long x, y;
    gcd(a, mod, x, y);
    return x < 0 ? (mod + x % mod) : (x % mod);
}

long fac[MAX];
long inv[MAX];
long po2[MAX];

void prepare() {
    fac[0] = fac[1] = inv[0] = inv[1] = 1;
    po2[0] = 1;
    po2[1] = 2;
    for (long n = 2; n < MAX; n++) {
        fac[n] = n * fac[n - 1] % MOD;
        inv[n] = invmod(fac[n], MOD);
        po2[n] = 2 * po2[n - 1] % MOD;
    }
}

inline long compute(int N, int M, int k) {
    long num = (fac[M] * fac[N - k] % MOD) * po2[k] % MOD;
    long den = (inv[k] * inv[M - k] % MOD) * inv[N - 2 * M] % MOD;
    return num * den % MOD;
}

auto solve() {
    int N, M;
    cin >> N >> M >> ws;
    N *= 2; // adjust N

    long Q = fac[N] * inv[N - 2 * M] % MOD;
    long i = -1;
    for (int k = 1; k <= M; k++) {
        Q = (Q + i * compute(N, M, k) + MOD) % MOD;
        i = -i;
    }
    return Q * fac[N - 2 * M] % MOD;
}

// *****

int main() {
    prepare();
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
