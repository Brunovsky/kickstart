#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 10010

int N;
long row[MAXN];
long col[MAXN];

auto solve() {
    cin >> N;
    for (int i = 0; i < N; i++) {
        cin >> row[i] >> col[i];
    }

    long minR = *min_element(row, row + N);
    long maxR = *max_element(row, row + N);
    long minC = *min_element(col, col + N);
    long maxC = *max_element(col, col + N);

    long R = (minR + maxR) / 2;
    long C = (minC + maxC) / 2;

    long steps = 0;
    for (int i = 0; i < N; i++) {
        long r = row[i], c = col[i];
        steps = max(steps, (abs(r + c - (R + C)) + abs(r - c - (R - C))) / 2);
    }

    return steps;
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
