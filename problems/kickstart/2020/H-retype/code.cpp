#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    long N, K, S;
    cin >> N >> K >> S;
    // restart
    long restart = K + N;
    long back = K - 1 + (K - S) + N - S + 1;
    return min(restart, back);
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
