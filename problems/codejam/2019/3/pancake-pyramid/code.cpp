#include <bits/stdc++.h>

using namespace std;

// *****

constexpr long mod = 1'000'000'007;

#define MAXS 1'000'010

int S, M;
long P[MAXS];
long cum[MAXS];
long prior[MAXS];  // [i] = max{j<i : P[j]>=P[i]}
long sprior[MAXS]; // [i] = max{j<i : P[j]>P[i]}
long after[MAXS];  // [i] = max{j>i : P[i]<=P[j]}
long safter[MAXS]; // [i] = max{j>i : P[i]<P[j]}

long pancakes(int i, int j) {
    return (cum[j - 1] - cum[i]) % mod;
}

auto solve() {
    cin >> S;
    for (int i = 1; i <= S; i++) {
        cin >> P[i];
        cum[i] = (cum[i - 1] + P[i]) % mod;
    }
    sprior[0] = prior[0] = 0;
    for (int i = 1; i <= S; i++) {
        int j = i - 1;
        while (j != 0 && P[j] < P[i])
            j = prior[j];
        prior[i] = j;
        sprior[i] = P[i] == P[j] ? sprior[j] : j;
    }
    safter[S + 1] = after[S + 1] = S + 1;
    for (int i = S; i >= 1; i--) {
        int j = i + 1;
        while (j != S + 1 && P[i] > P[j])
            j = after[j];
        after[i] = j;
        safter[i] = P[i] == P[j] ? safter[j] : j;
    }
    long sum = 0;
    for (int i = 1; i <= S; i++) {
        if (after[i] == S + 1)
            continue;
        long left = i - sprior[i];
        long right = S + 1 - after[i];
        long combinations = left * right % mod;
        long dist = after[i] - i - 1;
        long overflow = (dist * P[i] - pancakes(i, after[i])) % mod;
        sum = (sum + combinations * overflow) % mod;
        // printf("after[%d]: %ld\n", i, combinations * overflow);
    }
    for (int i = S; i >= 1; i--) {
        if (prior[i] == 0)
            continue;
        long left = sprior[i];
        long right = safter[i] - i;
        long combinations = left * right % mod;
        long dist = i - prior[i] - 1;
        long overflow = (dist * P[i] - pancakes(prior[i], i)) % mod;
        sum = (sum + combinations * overflow) % mod;
        // printf("prior[%d]: %ld\n", i, combinations * overflow);
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
 *
 *    X                                   X
 *    X                                   X
 *    X                                   X
 *    X     X     X     X     X     X     X
 *    X     X     X     X     X     X     X
 *    X     X     X     X     X     X     X
 *    X     X     X     X     X     X     X
 *    X     X     X     X     X     X     X
 *    X     X     X     X     X     X     X
 *    ^           ^     ^     ^           ^
 * sprior[i]  prior[i]  i  after[i]  safter[i]
 *
 */
