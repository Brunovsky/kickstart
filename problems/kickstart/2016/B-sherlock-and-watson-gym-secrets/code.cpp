#include <bits/stdc++.h>

using namespace std;

// *****

struct count_t {
    long a = 0, b = 0, c = 0;
};

using count_map_t = vector<count_t>;

constexpr long MOD = 1'000'000'007;
long A, B, N, K;

// compute b^e (mod m)
long power_mod(long b, long e, long m) {
    long power = 1;
    long mul = b % m;
    while (e > 0) {
        if (e & 1) {
            power = (power * mul) % m;
        }
        e >>= 1;
        mul = (mul * mul) % m;
    }
    return power;
}

// compute counts of residues b^e (mod m) for b=1,...,n
count_map_t count_residues(long n) {
    count_map_t map(K);
    for (long i = 1; i <= n; ++i) {
        long ia = power_mod(i, A, K);
        long ib = (K - power_mod(i, B, K)) % K;
        map[ia].a++;
        map[ib].b++;
        map[ia].c += ia == ib;
    }
    return map;
}

auto solve() {
    cin >> A >> B >> N >> K >> ws;
    if (K == 1) {
        return (N - 1) % MOD * (N % MOD) % MOD;
    }

    long intervals = (N / K) % MOD;
    long intervals2 = intervals * intervals % MOD;

    count_map_t all = count_residues(K);
    count_map_t tail = count_residues(N % K);

    long count = 0;
    for (long i = 0; i < K; ++i) {
        long a = all[i].a;
        long b = all[i].b;
        long c = all[i].c;
        long al = tail[i].a;
        long bl = tail[i].b;
        long cl = tail[i].c;
        long s1 = a * b % MOD * intervals2 % MOD;
        long s2 = al * b % MOD * intervals % MOD;
        long s3 = a * bl % MOD * intervals % MOD;
        long s4 = al * bl % MOD;
        long s5 = (c * intervals % MOD + cl) % MOD;
        count = (count + s1 + s2 + s3 + s4 - s5 + 7 * MOD) % MOD;
    }
    return count;
}

// |i    |   i |     |     | i':
// | jj  |   jj|     |     |j' :

// a * b * intervals^2 - overlaps * intervals
// a' * b * intervals
// a * b' * intervals
// a' * b' - overlaps

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
