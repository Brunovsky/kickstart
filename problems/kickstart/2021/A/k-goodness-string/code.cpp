#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N, K;
    string S;
    cin >> N >> K >> ws >> S >> ws;
    int score = 0;
    for (int i = 0; i < N / 2; i++) {
        score += S[i] != S[N - i - 1];
    }
    return abs(score - K);
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
