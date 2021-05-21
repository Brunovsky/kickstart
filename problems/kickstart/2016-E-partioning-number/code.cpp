#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    long N, D;
    cin >> N >> D >> ws;
    long count = (N % D) == 0;
    // put d balls in the first bucket
    for (long d = D; d < N; d += D) {
        long R = N - d; // there are R remaining balls
        long k = R / d; // suppose k more buckets
        while (k > 0) {
            long s = R - k * d; // surplus of balls
            if (s > 2 * k) {
                break; // busted, too many balls
            }
            if (s > k) {
                s -= 2 * (s - k); // +2 balls to the s-k tail buckets
            }
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
