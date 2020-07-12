#include <bits/stdc++.h>

using namespace std;

// *****

constexpr int min4(int a, int b, int c, int d) {
    return min(min(a, b), min(c, d));
}

auto solve() {
    int K;
    cin >> K;
    vector<int> A(K);
    for (int i = 0; i < K; ++i)
        cin >> A[i];

    int n1 = 0, n2 = 0, n3 = 0, n4 = 0;

    for (int i = 1; i < K; ++i) {
        if (A[i - 1] == A[i]) {
            int m1 = min4(n1, n2 + 1, n3 + 1, n4 + 1);
            int m2 = min4(n1 + 1, n2, n3 + 1, n4 + 1);
            int m3 = min4(n1 + 1, n2 + 1, n3, n4 + 1);
            int m4 = min4(n1 + 1, n2 + 1, n3 + 1, n4);
            n1 = m1;
            n2 = m2;
            n3 = m3;
            n4 = m4;
        }

        if (A[i - 1] < A[i]) {
            int m1 = min4(n1 + 1, n2 + 1, n3 + 1, n4 + 1);
            int m2 = min4(n1, n2 + 1, n3 + 1, n4 + 1);
            int m3 = min4(n1, n2, n3 + 1, n4 + 1);
            int m4 = min4(n1, n2, n3, n4 + 1);
            n1 = m1;
            n2 = m2;
            n3 = m3;
            n4 = m4;
        }

        if (A[i - 1] > A[i]) {
            int m1 = min4(n1 + 1, n2, n3, n4);
            int m2 = min4(n1 + 1, n2 + 1, n3, n4);
            int m3 = min4(n1 + 1, n2 + 1, n3 + 1, n4);
            int m4 = min4(n1 + 1, n2 + 1, n3 + 1, n4 + 1);
            n1 = m1;
            n2 = m2;
            n3 = m3;
            n4 = m4;
        }
    }

    return min4(n1, n2, n3, n4);
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
