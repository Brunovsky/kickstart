#include <bits/stdc++.h>

using namespace std;

// *****

int N, K;
vector<long long> V;
vector<long long> suffix;

auto solve() {
    cin >> N >> K >> ws;
    V.resize(N);
    suffix.assign(N + 1, 0);
    for (int i = 0; i < N; i++) {
        cin >> V[i];
    }
    sort(begin(V), end(V));
    for (int i = N - 1; i >= 0; i--) {
        suffix[i] = suffix[i + 1] + V[i];
    }

    double expected = 1.0 * suffix[0] / N;
    for (int i = 1; i <= K; i++) {
        int over = ceil(expected);
        int j = lower_bound(begin(V), end(V), over) - begin(V);
        double expected_over = 1.0 * suffix[j] / (N - j);
        expected = (expected * j + expected_over * (N - j)) / N;
    }
    return expected;
}

// *****

int main() {
    unsigned T;
    cout << fixed << showpoint << setprecision(7);
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
