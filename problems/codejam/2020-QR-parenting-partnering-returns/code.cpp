#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N;
    cin >> N >> ws;

    vector<int> S(N), E(N), O(N);
    for (int i = 0; i < N; ++i)
        cin >> S[i] >> E[i] >> ws, O[i] = i;

    sort(O.begin(), O.end(), [&](int l, int r) {
        if (S[l] != S[r])
            return S[l] < S[r];
        return E[l] < E[r];
    });

    string possible(N, '.');
    int C = -1, J = -1;

    for (int i = 0; i < N; ++i) {
        int o = O[i], s = S[o], e = E[o];
        if (s >= C) {
            possible[o] = 'C';
            C = e;
        } else if (s >= J) {
            possible[o] = 'J';
            J = e;
        } else {
            return "IMPOSSIBLE"s;
        }
    }

    return possible;
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
