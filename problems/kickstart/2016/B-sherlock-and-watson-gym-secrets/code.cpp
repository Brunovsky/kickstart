#include <bits/stdc++.h>

using namespace std;

// *****

using i64 = int64_t;

struct count_t {
    i64 a = 0, b = 0, c = 0;
};

using count_map_t = vector<count_t>;

constexpr i64 MOD = 1'000'000'007;
i64 A, B, N, K;

// compute b^e (mod m)
i64 power_mod(i64 b, i64 e, i64 m) {
    i64 power = 1;
    i64 mul = b % m;
    while (e > 0) {
        if (e & 1)
            power = (power * mul) % m;
        e >>= 1;
        mul = (mul * mul) % m;
    }
    return power;
}

// compute counts of residues b^e (mod m) for b=1,...,n
count_map_t count_residues(i64 n) {
    count_map_t map(K);
    for (i64 i = 1; i <= n; ++i) {
        i64 ia = power_mod(i, A, K);
        i64 ib = (K - power_mod(i, B, K)) % K;
        ++map[ia].a;
        ++map[ib].b;
        if (ia == ib)
            ++map[ia].c;
    }
    return map;
}

auto solve() {
    cin >> A >> B >> N >> K >> ws;
    if (K == 1) // if fucking hate you sometimes, google
        return (N - 1) % MOD * (N % MOD) % MOD;

    i64 intervals = (N / K) % MOD;
    i64 intervals2 = intervals * intervals % MOD;

    count_map_t all = count_residues(K);
    count_map_t tail = count_residues(N % K);

    i64 count = 0;
    for (i64 i = 0; i < K; ++i) {
        i64 a = all[i].a;
        i64 b = all[i].b;
        i64 c = all[i].c;
        i64 al = tail[i].a;
        i64 bl = tail[i].b;
        i64 cl = tail[i].c;
        i64 s1 = a * b % MOD * intervals2 % MOD;
        i64 s2 = al * b % MOD * intervals % MOD;
        i64 s3 = a * bl % MOD * intervals % MOD;
        i64 s4 = al * bl % MOD;
        i64 s5 = (c * intervals % MOD + cl) % MOD;
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
