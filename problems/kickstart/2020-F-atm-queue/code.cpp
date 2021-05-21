#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N, X;
    cin >> N >> X >> ws;
    vector<pair<long, int>> A(N);
    for (int i = 0; i < N; i++) {
        long want;
        cin >> want;
        A[i].first = (want + X - 1) / X;
        A[i].second = i + 1;
    }
    stable_sort(begin(A), end(A));
    for (int i = 0; i < N; i++)
        cout << ' ' << A[i].second;
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ":";
        solve();
        cout << endl;
    }
    return 0;
}
