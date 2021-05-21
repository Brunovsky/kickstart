#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    string s;
    cin >> s;
    int S = s.size();
    int flips = 0;
    for (int i = 1; i < S; i++) {
        flips += s[i] != s[i - 1];
    }
    flips += s[S - 1] == '-';
    return flips;
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
