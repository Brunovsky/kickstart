#include <bits/stdc++.h>

using namespace std;

// *****

struct ratio_t {
    long num;
    long den;
};

struct hash_ratio {
    size_t operator()(ratio_t r) const noexcept {
        return hash<long>{}((r.num << 8) | r.den) ^ 0xfe7379ab;
    }
};

bool operator==(ratio_t lhs, ratio_t rhs) {
    return lhs.num == rhs.num && lhs.den == rhs.den;
}

#define MAXN 10000
#define MAXD 51

long N, D;
unordered_map<long, long> cntA;
unordered_set<ratio_t, hash_ratio> targets;
long A[MAXN];

long gcd(long a, long b) {
    while (a != 0) {
        b = b % a;
        swap(a, b);
    }
    return abs(b);
}

ratio_t ratio_of(long n, long d) {
    long g = gcd(n, d);
    return {n / g, d / g};
}

// are the slices too small?
bool have_enough_pizza(long n, long d) {
    long cnt = 0;
    for (long i = 0; i < N; i++) {
        if (d * A[i] < n) {
            return false;
        }
        cnt += d * A[i] / n;
        if (cnt >= D) {
            return true;
        }
    }
    return false;
}

long optimize_multiples(long need, long k, long n, long d) {
    // can't make a perfect split
    if (k * d > need) {
        return 0;
    }
    long cntk = min(cntA[k * n], need / (k * d));
    long perfect = cntk;
    for (long i = 0; i <= cntk; i++) {
        perfect = max(perfect, i + optimize_multiples(need - k * d * i, k + 1, n, d));
    }
    return perfect;
}

auto solve() {
    cin >> N >> D >> ws;
    targets.clear();
    cntA.clear();
    memset(A, 0, sizeof(A));
    for (long i = 0; i < N; i++) {
        cin >> A[i];
        if (++cntA[A[i]] == D) {
            while (++i < N) {
                cin >> A[i];
            }
            return 0L;
        }
        for (long d = 1; d <= D; d++) {
            targets.insert(ratio_of(A[i], d));
        }
    }
    sort(A, A + N, greater<long>{});

    long best = D - 1;

    for (auto target : targets) {
        long n = target.num, d = target.den;
        if (have_enough_pizza(n, d)) {
            long perfect = optimize_multiples(D - (D % d), 1, n, d);
            best = min(best, D - perfect);
        }
    }

    return best;
}

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
