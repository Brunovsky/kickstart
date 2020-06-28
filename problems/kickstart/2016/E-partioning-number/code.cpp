#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    uint64_t N, D;
    cin >> N >> D >> ws;
    uint64_t count = (N % D) == 0;
    // put d balls in the first bucket
    for (uint64_t d = D; d < N; d += D) {
        uint64_t R = N - d; // there are R remaining balls
        uint64_t k = R / d; // suppose k more buckets
        while (k > 0) {
            uint64_t s = R - k * d; // surplus of balls
            if (s > 2 * k)
                break; // busted, too many balls
            if (s > k)
                s -= 2 * (s - k); // +2 balls to the s-k tail buckets
            count += 1 + s / 2;
            --k;
        }
    }
    return count;
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
