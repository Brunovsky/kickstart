#include <bits/stdc++.h>

using namespace std;

// *****

/**
 * Given n + k coin tosses, what is the probability that exactly k coin tosses
 * are heads?
 *
 * answer:       1      (n+k)
 *            ------- * (   )
 *            2^(n+k)   ( n )
 *
 * (0, 0) --> 1
 * (1, 1) --> 0.5
 * (2, 0) --> 0.25
 */
double single_probability(int n, int k) {
    double p = 1.0;
    int s = n + k;

    k = min(n, k);
    while (k > 0) {
        p *= n-- + k;
        p /= k--;
    }
    while (s-- > 0) {
        p /= 2.0;
    }

    return p;
}

/**
 * what is the accumulated sum of the previous formula over k = 0, 1, ..., K ?
 *
 * answer:       1    (                                              )
 *              --- * ( 1 + (n+1)/2 + (n+1)(n+2)/4                   )
 *              2^n   (                                              )
 */
double accumulated_probability(int n, int k) {}

auto solve() {
    int n, k;
    cin >> n >> k;
    return single_probability(n, k);
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
 * X           .  0
 *    .        .  1
 *  .          .  2
 *  .          X  3
 *             .  4
 *             .
 *             .
 *             .
 *
 *       n+1
 *
 *
 *
 * (n, i)
 * ()
 */
