#include <bits/stdc++.h>

using namespace std;

// *****

constexpr long mod = 1'000'000'007;

#define MAXS 1'000'010

int S, M;
long P[MAXS];

auto solve() {
    cin >> S;
    for (int i = 0; i < S; i++) {
        cin >> P[i];
    }
    long sum = 0;
    for (int l = 0; l + 2 < S; l++) {
        for (int r = l + 3; r <= S; r++) {
            int m = max_element(P + l, P + r) - P;
            long highest = 0;
            for (int i = l; i < m; i++) {
                highest = max(highest, P[i]);
                sum = (sum + highest - P[i]) % mod;
            }
            highest = 0;
            for (int i = r - 1; i > m; i--) {
                highest = max(highest, P[i]);
                sum = (sum + highest - P[i]) % mod;
            }
        }
    }
    return (sum + mod) % mod; // make sure sum is positive
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

/**
 *         X
 *   X     X
 *   X   X X
 *   X   X X
 *   X   X X
 *   X X X X
 * X X X X X X
 *
 *
 * X   X
 * X X X
 */
