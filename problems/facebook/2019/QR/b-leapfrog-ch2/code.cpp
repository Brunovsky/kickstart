#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    string s;
    cin >> s;
    int cntB = 0;
    for (char c : s)
        cntB += c == 'B';
    int S = s.size() - 1;
    if (S == 2)
        return (cntB == 1) ? "Y" : "N";
    else
        return (2 <= cntB && cntB < S) ? "Y" : "N";
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
