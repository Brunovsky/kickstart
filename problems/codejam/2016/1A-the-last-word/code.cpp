#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    string s;
    cin >> s;
    int S = s.size();
    char c = 0;
    string ans;
    for (int i = 0; i < S; i++) {
        ans = s[i] >= c ? s[i] + ans : ans + s[i];
        c = max(c, s[i]);
    }
    return ans;
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
