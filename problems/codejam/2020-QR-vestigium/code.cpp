#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N;
    cin >> N;
    vector<vector<int>> matrix(N, vector<int>(N));
    for (int r = 0; r < N; ++r)
        for (int c = 0; c < N; ++c)
            cin >> matrix[r][c];

    int trace = 0, r_repeated = 0, c_repeated = 0;

    vector<bool> mask(N + 1, false);
    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            int k = matrix[r][c];
            if (mask[k]) {
                ++r_repeated;
                break;
            }
            mask[k] = true;
        }
        mask.assign(N + 1, false);
    }
    for (int c = 0; c < N; ++c) {
        for (int r = 0; r < N; ++r) {
            int k = matrix[r][c];
            if (mask[k]) {
                ++c_repeated;
                break;
            }
            mask[k] = true;
        }
        mask.assign(N + 1, false);
    }
    for (int n = 0; n < N; ++n)
        trace += matrix[n][n];

    return to_string(trace) + " " + to_string(r_repeated) + " " +
           to_string(c_repeated);
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
