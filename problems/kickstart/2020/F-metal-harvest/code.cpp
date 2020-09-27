#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N;
    long K;
    cin >> N >> K;
    vector<array<long, 2>> vec(N);
    for (int i = 0; i < N; i++) {
        cin >> vec[i][0] >> vec[i][1];
    }
    sort(begin(vec), end(vec));

    int deployments = 0;
    long start = -K;

    for (int i = 0; i < N; i++) {
        if (start + K >= vec[i][1])
            continue;

        deployments++;
        start = max(start + K, vec[i][0]);
        while (start + K < vec[i][1])
            deployments++, start += K;
    }

    return deployments;
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
