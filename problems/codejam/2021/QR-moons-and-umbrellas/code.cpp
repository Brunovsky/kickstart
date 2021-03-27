#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int X, Y; // X=CJ, Y=JC
    string s;
    cin >> X >> Y >> s;
    int S = s.size();
    int inf = INT_MAX / 2;
    int c = s[0] == 'J' ? inf : 0;
    int j = s[0] == 'C' ? inf : 0;
    for (int i = 1; i < S; i++) {
        int cn = s[i] == 'J' ? inf : min(c, j + Y);
        int jn = s[i] == 'C' ? inf : min(c + X, j);
        c = cn;
        j = jn;
    }
    return min(c, j);
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
