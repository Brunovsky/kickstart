#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N;
    cin >> N >> ws;
    vector<int> V(N);
    for (int i = 0; i < N; ++i) {
        cin >> V[i];
    }

    int maximum = -1;
    int records = 0;
    for (int i = 0; i < N; ++i) {
        if (V[i] > maximum && (i + 1 == N || V[i] > V[i + 1])) {
            ++records;
        }
        maximum = max(V[i], maximum);
    }
    return records;
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
