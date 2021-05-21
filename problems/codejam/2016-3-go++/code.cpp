#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N, L;
    cin >> N >> L;
    vector<string> G(N);
    string B;
    for (int i = 0; i < N; i++)
        cin >> G[i];
    cin >> B;
    for (int i = 0; i < N; i++)
        if (B == G[i])
            return "IMPOSSIBLE"s;
    string a, b;
    for (char c : B) {
        if (c == '0')
            a += "10", b += "1?";
        else
            a += "01", b += "0?";
    }
    a.pop_back();
    return a + " " + b;
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
