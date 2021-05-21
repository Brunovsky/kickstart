#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    string pattern;
    getline(cin, pattern);
    const long P = pattern.size();
    long I, J;
    cin >> I >> J >> ws;
    ----I, --J;

    long B = 0, IB = 0, JB = 0;
    for (int i = 0; i < P; ++i) {
        B += pattern[i] == 'B';
        if (i == (I % P)) {
            IB = B;
        }
        if (i == (J % P)) {
            JB = B;
        }
    }

    return (J / P) * B + JB - ((I / P) * B + IB);
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
