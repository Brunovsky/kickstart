#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int S, K;
    string s;
    cin >> s >> K;
    S = s.size();

    int flips = 0;
    for (int i = 0; i <= S - K; i++) {
        if (s[i] == '-') {
            flips++;
            for (int j = i; j < i + K; j++) {
                s[j] = s[j] == '-' ? '+' : '-';
            }
        }
    }

    for (int i = S - K + 1; i < S; i++) {
        if (s[i] == '-') {
            return "IMPOSSIBLE"s;
        }
    }

    return to_string(flips);
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
