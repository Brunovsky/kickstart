#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N;
    cin >> N;
    vector<int> A(N + 1);
    for (int i = 1; i <= N; i++)
        cin >> A[i];

    int M = N - 1;
    vector<long> D(M + 2, LONG_MAX / 2);
    vector<int> next(M + 2, M + 1);
    vector<int> prev(M + 2, 0);
    for (int i = 1; i <= M; i++)
        D[i] = A[i + 1] - A[i];

    for (int i = 1; i <= M;) {
        int j = i + 1;
        while (j <= M && D[i] == D[j])
            j++;
        for (int k = i; k < j; k++)
            next[k] = j;
        i = j;
    }
    for (int i = M; i >= 1;) {
        int j = i - 1;
        while (j >= 1 && D[i] == D[j])
            j--;
        for (int k = i; k > j; k--)
            prev[k] = j;
        i = j;
    }

    int best = max(next[2] - 1, M - prev[M - 1]);

    for (int i = 1; i <= M; i++) {
        best = max(best, next[i] - prev[i] - 1);
        best = max(best, next[i + 1] - i);
        best = max(best, i - prev[i - 1]);
    }

    for (int i = 1; i < M; i++) {
        long k = D[i] + D[i + 1];
        int L = k == 2 * D[i - 1] ? (i - 1 - prev[i - 1]) : 0;
        int R = k == 2 * D[i + 2] ? (next[i + 2] - i - 2) : 0;
        best = max(best, 2 + L + R);
    }

    return best + 1;
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
