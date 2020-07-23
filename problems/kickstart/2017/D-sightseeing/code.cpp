#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    long N, Ts, Tf;
    cin >> N >> Ts >> Tf;

    // dp[i]: earliest we can be at THIS city after sightseeing i times.
    vector<long> dp(N, INT64_MAX);
    dp[0] = 0;

    for (long i = 1; i < N; ++i) {
        long S, F, D;
        cin >> S >> F >> D >> ws;
        for (long k = i; k > 0; --k) {
            dp[k] = min(dp[k], dp[k - 1] + Ts);
        }
        for (long k = 0; k <= i; ++k) {
            long x = (dp[k] <= S) ? 0 : (dp[k] - S + F - 1) / F;
            dp[k] = S + x * F + D;
        }
    }

    if (dp[0] > Tf) {
        return "IMPOSSIBLE"s;
    }

    long i = 0;
    while (i + 1 < N && dp[i + 1] <= Tf) {
        ++i;
    }

    return to_string(i);
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
