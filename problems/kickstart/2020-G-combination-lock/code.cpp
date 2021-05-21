#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXW 200001

int N, W;
long X[MAXW], A[MAXW];

long get_cost(int i, long M) {
    int j = i + W;
    int m = lower_bound(X + i, X + j, M) - X;
    long cost_left = (m - i) * M - (A[m] - A[i]);
    long cost_right = (A[j] - A[m]) - (j - m) * M;
    return cost_left + cost_right;
}

auto solve() {
    cin >> W >> N;
    memset(X, 0, sizeof(X));
    memset(A, 0, sizeof(A));
    for (int i = 0; i < W; i++) {
        cin >> X[i], X[i]--;
    }

    sort(X, X + W);
    for (int i = 0; i < W; i++) {
        X[i + W] = X[i] + N;
    }
    for (int i = 0; i < 2 * W; i++) {
        A[i + 1] = A[i] + X[i];
    }

    long answer = LONG_MAX;

    for (int i = 0; i < W; i++) {
        long L = X[i], R = X[i + W - 1];
        while (L <= R) {
            long M1 = (2 * L + R) / 3;
            long M2 = (L + 2 * R + 2) / 3;
            if (get_cost(i, M1) < get_cost(i, M2)) {
                R = M2 - 1;
            } else {
                L = M1 + 1;
            }
        }
        answer = min(answer, min(get_cost(i, L), get_cost(i, R)));
    }

    return answer;
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
