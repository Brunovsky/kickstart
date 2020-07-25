#include <bits/stdc++.h>

using namespace std;

// *****

struct ratio_t {
    long num, den;
};

long gcd(long a, long b) {
    while (a != 0) {
        b = b % a;
        swap(a, b);
    }
    return abs(b);
}

bool operator==(ratio_t lhs, ratio_t rhs) {
    return lhs.num == rhs.num && lhs.den == rhs.den;
}

struct hasher {
    size_t operator()(ratio_t r) const noexcept {
        if (r.num == LONG_MAX) {
            return ULONG_MAX;
        } else {
            return 0x9e3779b9 + ((r.num >> 2) ^ (r.den << 6));
        }
    }
};

ratio_t ratio_of(long n, long d) {
    n = abs(n), d = abs(d);
    long g = gcd(n, d);
    n /= g, d /= g;
    return {n, d};
}

#define MAXN 300

int N;
long C[MAXN];
long J[MAXN];
unordered_set<ratio_t, hasher> quot;

auto solve() {
    quot.clear();
    cin >> N;
    for (int i = 0; i < N; i++) {
        cin >> C[i] >> J[i];
    }
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            if ((C[i] < C[j] && J[i] > J[j]) || (C[i] > C[j] && J[i] < J[j])) {
                quot.insert(ratio_of(C[i] - C[j], J[i] - J[j]));
            }
        }
    }
    return quot.size() + 1;
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
