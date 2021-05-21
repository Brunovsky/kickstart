#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N, X, K, A, B, C;
    cin >> N >> X >> K >> A >> B >> C;

    double expected = 0.0;

    for (int b = 1; b <= max(X, K); b <<= 1) {
        int x = b & X, k = b & K;
        double d = x ? 1 : 0;
        for (int i = 0; i < N; i++) {
            d = (k ? d : 0) * A // AND
                + (k ? 1 : d) * B // OR
                + (k ? 1 - d : d) * C; // XOR
            d /= 100;
        }
        expected += d * b;
    }

    return expected;
}

// *****

int main() {
    cout << setprecision(10) << fixed;
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
