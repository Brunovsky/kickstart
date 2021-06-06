#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int R, C, W;
    cin >> R >> C >> W;
    int k = C / W;
    return R * k + (W - 1) + (k * W == C ? 0 : 1);
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
