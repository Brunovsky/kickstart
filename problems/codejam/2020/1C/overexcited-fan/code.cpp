#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int X, Y;
    cin >> X >> Y;
    string M;
    cin >> M;
    int m = M.size();
    for (int i = 0; i < m; i++) {
        if (M[i] == 'N') {
            Y++;
        } else if (M[i] == 'S') {
            Y--;
        } else if (M[i] == 'E') {
            X++;
        } else if (M[i] == 'W') {
            X--;
        }
        if (abs(X) + abs(Y) <= i + 1) {
            return to_string(i + 1);
        }
    }
    return "IMPOSSIBLE"s;
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
