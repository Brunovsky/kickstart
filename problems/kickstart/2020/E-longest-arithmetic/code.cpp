#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N;
    cin >> N;
    vector<long> A(N + 1);
    for (int i = 0; i < N; i++) {
        cin >> A[i];
    }

    int len = 2;

    for (int i = 0; i + 1 < N;) {
        long dif = A[i] - A[i + 1];
        int start = i;
        do {
            i++;
        } while (i + 1 < N && A[i] - A[i + 1] == dif);
        len = max(len, i - start + 1);
    }

    return len;
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
