#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N;
    cin >> N;
    vector<int> m(N);
    for (int i = 0; i < N; i++)
        cin >> m[i];

    // first method
    long A = 0;
    for (int i = 0; i + 1 < N; i++)
        A += max(0, m[i] - m[i + 1]);

    // second method
    int rate = 0;
    for (int i = 0; i + 1 < N; i++)
        rate = max(rate, m[i] - m[i + 1]);

    long B = 0;
    for (int i = 0; i + 1 < N; i++)
        B += min(rate, m[i]);

    return to_string(A) + ' ' + to_string(B);
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
