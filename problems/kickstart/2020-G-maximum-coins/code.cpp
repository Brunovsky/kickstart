#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 1000

int N;
long S[2 * MAXN - 1];

auto solve() {
    cin >> N;
    memset(S, 0, sizeof(S));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int coins;
            cin >> coins;
            S[i - j + N] += coins;
        }
    }
    return *max_element(S, S + 2 * N);
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
