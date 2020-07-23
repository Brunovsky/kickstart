#include <bits/stdc++.h>

using namespace std;

// *****

ulong N;
vector<ulong> A;

auto solve() {
    cin >> N >> ws;
    A.assign(N, 0);
    for (ulong i = 0; i < N; ++i) {
        cin >> A[i] >> ws;
    }

    sort(begin(A), end(A));

    ulong count = 0;
    ulong zeros = 0;

    // deal with 0s with basic combinatorics
    while (zeros < N && A[zeros] == 0) {
        zeros++;
    }
    if (zeros > 1) {
        count = (N - zeros) * zeros * (zeros - 1) / 2; // 0 = 0*x, x>0
        if (zeros > 2) {
            count += zeros * (zeros - 1) * (zeros - 2) / 6; // 0 = 0*0
        }
    }

    // now deal with positives...
    for (ulong i = zeros; i + 2 < N; ++i) {
        for (ulong j = i + 1; j + 1 < N; ++j) {
            ulong P = A[i] * A[j];
            auto left = begin(A) + j + 1, right = end(A);
            auto hi = upper_bound(left, right, P);
            auto lo = lower_bound(left, right, P);
            count += ulong(hi - lo);
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
