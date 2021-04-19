#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N;
    string S;
    cin >> N >> S;
    cout << 1;
    for (int i = 1, len = 1; i < N; i++) {
        S[i - 1] < S[i] ? ++len : (len = 1);
        cout << ' ' << len;
    }
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ": ";
        solve();
        cout << endl;
    }
    return 0;
}
