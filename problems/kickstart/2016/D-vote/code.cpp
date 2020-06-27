#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N, M;
    cin >> N >> M >> ws;
    return double(N - M) / double(N + M);
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
