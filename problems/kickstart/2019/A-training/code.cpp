#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N, P;
    cin >> N >> P >> ws;
    vector<int> S(N, 0);
    for (int i = 0; i < N; ++i) {
        cin >> S[i] >> ws;
    }
    sort(begin(S), end(S));

    int l = 0, r = P - 1;
    int training = 0, min_training = 0;
    for (int i = 0; i < P; ++i) {
        training += S[r] - S[i];
    }

    min_training = training;
    ++l, ++r;
    while (r < N) {
        training += (P - 1) * (S[r] - S[r - 1]) - (S[r - 1] - S[l - 1]);
        min_training = min(min_training, training);
        ++l, ++r;
    }
    return min_training;
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
