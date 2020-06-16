#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N, K;
    vector<int> A;
    cin >> N >> K;
    A.resize(N);
    for (int i = 0; i < N; ++i)
        cin >> A[i];

    int count = 0;
    for (int i = 0; i < N - K + 1; ++i) {
        for (int j = i, n = K; j < i + K; ++j, --n)
            if (A[j] != n)
                goto next;
        ++count;
    next:;
    }

    return count;
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
