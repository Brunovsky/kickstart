#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    uint64_t L, R, N;
    cin >> L >> R >> ws;
    N = min(L, R);
    return N * (N + 1) / 2;
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
