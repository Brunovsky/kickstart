#include <bits/stdc++.h>

using namespace std;

// *****

#define CHAR(n) char('A' + P[n][1])
#define CNT(n)  P[n][0]

auto solve() {
    int N;
    cin >> N;
    vector<array<int, 2>> P(N);
    for (int i = 0; i < N; i++)
        cin >> P[i][0], P[i][1] = i;
    sort(begin(P), end(P));
    int a = N - 2, b = N - 1;
    while (CNT(a) < CNT(b)) {
        cout << ' ' << CHAR(b), CNT(b)--;
    }
    for (int i = 0; i < a; i++) {
        while (CNT(i)) {
            cout << ' ' << CHAR(i), CNT(i)--;
        }
    }
    while (CNT(a)) {
        cout << ' ' << CHAR(a) << CHAR(b), CNT(a)--;
    }
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ":";
        solve();
        cout << '\n';
    }
    return 0;
}
